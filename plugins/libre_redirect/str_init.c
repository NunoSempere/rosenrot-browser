void str_init(char* str, int n)
{
    for (int i = 0; i < n; i++)
        str[i] = ' ';
    str[n] = '\0';
} // could also use <https://manpages.ubuntu.com/manpages/impish/man3/strinit.3pub.html>

