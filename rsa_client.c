#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define buffer_size 1024

int power(long long x, unsigned int y, int p)
{
    int res = 1;
    x = x % p;
    if (x == 0)
        return 0;

    while (y > 0)
    {
        // If y is odd, multiply x with result
        if (y & 1)
            res = (res * x) % p;

        y = y >> 1; // y = y/2
        x = (x * x) % p;
    }
    return res;
}

int main()
{
    int clifd;
    struct sockaddr_in clientinfo;
    char rbuff[buffer_size];
    int P, G, x, y, kb;

    if ((clifd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Socket Creation Failed\n");
    else
        printf("\n Socket Created Successfully\n");

    clientinfo.sin_family = AF_INET;
    clientinfo.sin_port = htons(5007);
    clientinfo.sin_addr.s_addr = inet_addr("192.168.30.131");

    if (bind(clifd, (struct sockaddr *)&clientinfo, sizeof(clientinfo)) == -1)
        perror("Binding Failed\n");
    else
        printf("\n Binding is Successful\n");

    if (connect(clifd, (struct sockaddr *)&clientinfo, sizeof(clientinfo)) == -1)
        perror("Connect Failed\n");
    else
        printf("\n Connect Successful\n");

    //Get First Message
    read(clifd, rbuff, sizeof(rbuff));
    printf("\n%s\n\n", rbuff);

    //Getting Public Keys
    char data[buffer_size];
    int e, N;
    if (read(clifd, data, sizeof(data)) == -1)
    {
        printf("Read Error 1\n");
    }
    e = atoi(data);

    if (read(clifd, data, sizeof(data)) == -1)
    {
        printf("Read Error 2\n");
    }
    N = atoi(data);
    printf("\nRecieved Public Keys: e: %d, N: %d\n\n", e, N);

    //Encryption
    char buffer[1024];
    int cipher_arr[1024];
    printf("Enter the message: ");
    scanf("%[^\n]%*c", buffer);

    printf("\nCipher Text: ");
    int i;
    for (i = 0; buffer[i] != '\0'; i++)
    {
        int m = (int)buffer[i];
        int cipher = power(m, e, N);
        cipher_arr[i] = cipher;
        printf("%d ", cipher);
    }
    printf("\n\n");

    //Sending Max length
    sprintf(data, "%d", i);
    write(clifd, &data, sizeof(data));

    //Sending Cipher Text
    for (int j = 0; buffer[j] != '\0'; j++)
    {
        char enc_string[1024];
        sprintf(enc_string, "%d", cipher_arr[j]);
        //printf("%d-->%s ", cipher_arr[j], enc_string);
        write(clifd, &enc_string, sizeof(enc_string));
        strcpy(enc_string, "");
    }

    shutdown(clifd, SHUT_RDWR);
    close(clifd);
}