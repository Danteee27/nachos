#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{

    int len;
    char filename[maxlen +1];
    if (Create("text.txt") == -1)
    {
        Print("hello");
    }
    else
    {
    // xuất thông báo tạo tập tin thành công
    }
    Halt();
}