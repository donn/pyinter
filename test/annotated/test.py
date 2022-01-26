def pyinter(fn):
    fn.__dict__["__pyinter_annotation__"] = True
    return fn


@pyinter
def add(a: int, b: int) -> int:
    return a + b
