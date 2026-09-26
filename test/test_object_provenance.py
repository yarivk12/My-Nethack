"""Check origin metadata against the repository's pre-import object catalog."""
from pathlib import Path
import re
import subprocess

R = Path(__file__).resolve().parents[1]
VANILLA_REF = '9191de7079624e94acdde19d07814a0f03ce9450'


def object_types(text):
    # Definitions are macro calls whose final argument is the stable identity.
    # Ignore comments, disabled/deferred definitions and macro definitions.
    text = re.sub(r'/\*[\s\S]*?\*/', '', text)
    text = re.sub(r'(?m)^#if 0\s*\n[\s\S]*?^#endif[^\n]*', '', text)
    text = re.sub(r'(?m)^#(?:[^\n]*\\\n)*[^\n]*', '', text)
    result = []
    for match in re.finditer(r'(?m)^([A-Z_]+)\(', text):
        if match[1] in ('MARKER', 'GENERIC'):
            continue
        depth, quoted, escaped, start = 1, False, False, match.end()
        for i in range(start, len(text)):
            char = text[i]
            if quoted:
                if escaped:
                    escaped = False
                elif char == '\\':
                    escaped = True
                elif char == '"':
                    quoted = False
            elif char == '"':
                quoted = True
            elif char == '(':
                depth += 1
            elif char == ')':
                depth -= 1
                if not depth:
                    break
        name = text[start:i].rsplit(',', 1)[-1].strip()
        if name in ('0', 'STRANGE_OBJECT'):
            continue
        assert re.fullmatch(r'[A-Z][A-Z0-9_]*', name), name
        result.append(name)
    assert len(result) == len(set(result)), 'duplicate object identity'
    return set(result)


baseline = subprocess.check_output(
    ['git', 'show', VANILLA_REF + ':include/objects.h'], cwd=R, text=True)
vanilla = object_types(baseline)
current = object_types((R / 'include/objects.h').read_text())
assert vanilla <= current
origin = (R / 'src/objects.c').read_text().split('object_origin(int otyp)', 1)[1]
vanilla_cases, rest = origin.split('return OBJ_ORIGIN_VANILLA;', 1)
custom_cases, fallback = rest.split('return OBJ_ORIGIN_CUSTOM;', 1)
cases = lambda text: re.findall(r'case ([A-Z][A-Z0-9_]*):', text)
assert set(cases(vanilla_cases)) == vanilla
assert set(cases(custom_cases)) == current - vanilla
assert len(cases(origin)) == len(current)
assert re.search(r'default:\s*return OBJ_ORIGIN_UNKNOWN;', fallback)
assert '#ifdef MAIL_STRUCTURES\n    case SCR_MAIL:\n#endif' in origin
assert not re.search(r'\b(?:TOOL_CLASS|EP_\w+|enhancement_\w+)\b', origin)
print(f'PASS object provenance: {len(vanilla)} vanilla identities, '
      f'{len(current - vanilla)} custom identities, unknown fallback, mail guard')
