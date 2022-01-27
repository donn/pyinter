def pyinter(fn):
    fn.__dict__["__pyinter_annotation__"] = True
    return fn


@pyinter
def math(a: int, b: int, add: bool = False) -> int:
    return a + b if add else a - b
