#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int port = 0;
char *path;

void loadConfig()
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("config.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (strstr(line, "PORT=") != NULL)
        {
            port = atoi(line + 5);
        }
        if (strstr(line, "PATH=") != NULL)
        {
            path = line + 5;
        }
    }
    fclose(fp);
    printf("File is loaded!! :)\n");
    printf("Port: %d\n", port);
    printf("Path: %s\n", path);
}

void setUpServer(char httpHeader[])
{
    FILE *fp;
    fp = fopen(strcat(path, "index.html"), "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        strcat(httpHeader, line);
    }
    fclose(fp);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char httpHeader[100000] = "HTTP/1.1 200 OK\r\n\n";
    loadConfig();
    setUpServer(httpHeader);
    printf("Header: %s\n", httpHeader);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }
    printf("\n------ Listening!!!! ------\n\n");
    while (1)
    {
        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        write(new_socket, httpHeader, strlen(httpHeader));
        printf("------ Data sent!!!! ------");
        close(new_socket);
    }
    return 0;
}
