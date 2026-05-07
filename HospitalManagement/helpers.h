void intToStr(int val, char* buf) 
{
    if (val == 0) 
    { 
        buf[0] = '0'; buf[1] = '\0'; 
        return; 
    }
    char tmp[16]; int i = 0;
    while (val > 0) 
    { 
        tmp[i++] = '0' + val % 10; 
        val /= 10;
    }
    for (int j = 0; j < i; j++) 
    {
        buf[j] = tmp[i - 1 - j];
    } 
    buf[i] = '\0';
}

// float to char* — no sprintf allowed
void floatToStr(float val, char* buf) 
{
    int whole = (int)val;
    int frac = (int)((val - whole) * 100 + 0.5f);
    char tmp[16]; intToStr(whole, tmp);
    int i = 0; 
    while (tmp[i]) 
    {
        buf[i] = tmp[i++];
    }
    buf[i++] = '.';
    if (frac < 10) 
    {
        buf[i++] = '0';
    }
    char f[8]; intToStr(frac, f);
    int j = 0; 
    while (f[j]) 
    {
        buf[i++] = f[j++];
    }
    buf[i] = '\0';
}