#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define buffer_size 1024

int min(int a1, int a2)
{
    if (a1 < a2)
    {
        return a1;
    }
    else
    {
        return a2;
    }
}

int check_relatively_prime(int e, int theta_N)
{
    int check_till = min(e, theta_N);
    for (int i = 2; i <= check_till; i++)
    {
        if (e % i == 0 && theta_N % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

//PS: This only works in our case because e and theta_N are coprime
int mod_inverse(int a, int m) //a*op(mod(n))=1
{
    int m0 = m;
    int y = 0, x = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        int q = a / m; //Quotient
        int t = m;

        m = a % m;
        a = t;
        t = y;

        y = x - q * y;
        x = t;
        //printf("a: %d, m: %d, q: %d,t: %d,x: %d,y: %d\n",a,m,q,t,x,y);
    }

    // Make x positive
    if (x < 0)
        x += m0;

    return x;
}

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
    int listenfd, connfd;
    struct sockaddr_in srvinfo, cliinfo;
    int len;
    char buff[buffer_size];
    int x, y;
    int ka;
    FILE *fp;

    //Creation of Socket
    //int socket(int domain, int type, int protocol);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Socket Creation Failed\n");
    else
        printf("\n Socket Created Successfully\n");

    // Associating sockaddr_in info

    srvinfo.sin_family = AF_INET;
    srvinfo.sin_port = htons(5007);
    srvinfo.sin_addr.s_addr = inet_addr("192.168.30.131");

    // Registering socket at IP/Internet Layer

    if (bind(listenfd, (struct sockaddr *)&srvinfo, sizeof(srvinfo)) == -1)
        perror("Binding Failed\n");
    else
        printf("\n Binding is Successful\n");

    listen(listenfd, 4);
    len = sizeof(srvinfo);
    connfd = accept(listenfd, (struct sockaddr *)&cliinfo, &len);

    //Sent First Message
    strcpy(buff, "Server says: Hello Client, sending 2 public values");
    write(connfd, buff, sizeof(buff));
    printf("\nSending First Message to Client#%d....\n\n", connfd);

    // Key Generation
    printf("\nGenerating keys for Client#%d......\n\n", connfd);
    //srand(time(0));
    //p=rand();
    int p, q, N, theta_N;
    p = 17;
    q = 11;
    N = p * q;
    theta_N = (p - 1) * (q - 1);

    int e = 2;
    for (int i = 0; i < theta_N; i++)
    { //while(1) also works
        if (check_relatively_prime(e, theta_N))
        {
            break;
        }
        e++;
    }
    printf("Public Keys: e: %d N: %d\n\n", e, N);

    //Private Key
    int d = mod_inverse(e, theta_N);
    printf("Private keys d: %d\n\n", d);

    //Sending Public Keys
    printf("Sending Public keys to Client#%d....\n\n", connfd);
    char data[buffer_size];
    sprintf(data, "%d", e);
    write(connfd, &data, sizeof(data));
    sprintf(data, "%d", N);
    write(connfd, &data, sizeof(data));

    //Read Max length
    if (read(connfd, data, sizeof(data)) == -1)
    {
        printf("Read Error 1\n");
    }
    int max_len = atoi(data);

    //Receive Cipher Text
    printf("\nRecieved Cipher Text: ");
    int cipher_arr[1024];
    for (int i = 0; i < max_len; i++)
    {
        if (read(connfd, data, sizeof(data)) == -1)
        {
            printf("Read Error: Cipher Text\n");
        }
        cipher_arr[i] = atoi(data);
        printf("%d ", cipher_arr[i]);
    }
    printf("\n\n");

    //Decipher Text
    printf("Deciphered Text: ");
    for (int i = 0; i < max_len; i++)
    {
        int decipher = power(cipher_arr[i], d, N);
        printf("%c", decipher);
    }
    printf("\n\n");

    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    //int m = 88;
    //printf("Message: %d\n", m);
    //int cipher = power(m, e, N);
    //printf("Cipher Text: %d\n", cipher);
    //int decipher = power(cipher, d, N);
    //printf("Decipher Text: %d\n", decipher);

    return 0;
}