def pyinter(fn):
    fn.__dict__["__pyinter_annotation__"] = True
    return fn


@pyinter
def function_with_help() -> str:
    """
    HELP_STR
    """
    return "Something went wrong."
