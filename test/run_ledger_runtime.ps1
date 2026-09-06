param(
    [Parameter(Mandatory=$true)][string]$OutputDirectory,
    [Parameter(Mandatory=$true)][string]$RecoverExecutable
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$out = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Path $out -Force | Out-Null

# Compile the actual production function bodies with fixture-only adapters.
# Extraction avoids production test hooks and unresolved-symbol linker flags.
function Get-FunctionText([string]$Path, [string]$Name) {
    $source = [IO.File]::ReadAllText((Join-Path $repo $Path))
    $pattern = '(?m)^(?:staticfn )?\w+\r?\n' + [regex]::Escape($Name) + '\([\s\S]*?^\}'
    $matches = [regex]::Matches($source, $pattern)
    if ($matches.Count -ne 1) { throw "Expected one definition of $Name in $Path" }
    $line = 1 + ($source.Substring(0, $matches[0].Index).Split("`n").Length - 1)
    return "#line $line `"$($Path.Replace('\','/'))`"`n" + $matches[0].Value + "`n"
}
$dungeon = [IO.File]::ReadAllText((Join-Path $repo 'src/dungeon.c'))
$proto = [regex]::Match($dungeon, '(?ms)^struct proto_dungeon \{.*?^\};').Value
$branchMacro = [regex]::Match($dungeon, '(?ms)^#define dlev_in_current_branch.*?\r?\n\r?\n').Value
if (!$proto -or !$branchMacro) { throw 'Missing production type or branch macro' }
$parts = @($proto, $branchMacro)
foreach ($name in @('ledger_no','maxledgerno','ledger_to_dnum','ledger_to_dlev','depth','find_branch','lev_by_name')) {
    $parts += Get-FunctionText 'src/dungeon.c' $name
}
$parts += Get-FunctionText 'src/files.c' 'recover_savefile'
[IO.File]::WriteAllText((Join-Path $out 'ledger_runtime_functions.h'), ($parts -join "`n"))
$test = Join-Path $PSScriptRoot 'test_ledger_runtime.c'
& cl /nologo /std:c11 /W4 /WX /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS /DWIN32 /DWIN32CON /I"$repo/include" /I"$repo/submodules/lua" /I"$out" /Fo"$out/ledger_runtime.obj" /Fe"$out/ledger_runtime.exe" $test
if ($LASTEXITCODE) { throw 'Ledger fixture compilation failed' }
Push-Location $out
try {
    & ./ledger_runtime.exe
    if ($LASTEXITCODE) { throw 'Ledger runtime fixtures failed' }
    & $RecoverExecutable -d $out stand badzero badnegative badlimit oldversion
    if ($LASTEXITCODE) { throw 'Standalone recover failed' }
    & ./ledger_runtime.exe --verify-standalone
    if ($LASTEXITCODE) { throw 'Standalone recovery verification failed' }
} finally { Pop-Location }
