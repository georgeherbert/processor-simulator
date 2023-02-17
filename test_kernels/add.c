char factorial(char n);
char multiply(char a, char b);

char _start()
{
    return factorial(3);
}

char factorial(char n)
{
    if (n == 0)
    {
        return 1;
    }
    else
    {
        return multiply(n, factorial(n - 1));
    }
}

char multiply(char a, char b)
{
    int result = 0;
    for (int i = 0; i < b; i++)
    {
        result += a;
    }
    return result;
}