#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

// tes fonctions ASM
size_t ft_strlen(const char *s);
char *ft_strcpy(char *dst, const char *src);
int ft_strcmp(const char *s1, const char *s2);
ssize_t ft_write(int fd, const void *buf, size_t count);
ssize_t ft_read(int fd, void *buf, size_t count);
char *ft_strdup(const char *s);

void test_strlen()
{
    printf("=== strlen ===\n");
    char *s = "hello world";

    printf("libc: %zu\n", strlen(s));
    printf("asm : %zu\n", ft_strlen(s));
}

void test_strcpy()
{
    printf("\n=== strcpy ===\n");
    char src[] = "copy me";
    char dst1[100];
    char dst2[100];

    strcpy(dst1, src);
    ft_strcpy(dst2, src);

    printf("libc: %s\n", dst1);
    printf("asm : %s\n", dst2);
}

void test_strcmp()
{
    printf("\n=== strcmp ===\n");

    char *a = "abc";
    char *b = "abd";

    printf("libc: %d\n", strcmp(a, b));
    printf("asm : %d\n", ft_strcmp(a, b));

    printf("libc: %d\n", strcmp("abc", "abc"));
    printf("asm : %d\n", ft_strcmp("abc", "abc"));

    printf("libc: %d\n", strcmp("abz", "abc"));
    printf("asm : %d\n", ft_strcmp("abz", "abc"));
}

void test_write()
{
    printf("\n=== write ===\n");

    errno = 0;
    ssize_t r1 = write(1, "libc write\n", 11);
    printf("libc ret: %zd errno: %d\n", r1, errno);

    errno = 0;
    ssize_t r2 = ft_write(1, "asm write\n", 10);
    printf("asm  ret: %zd errno: %d\n", r2, errno);

    // test erreur
    errno = 0;
    r1 = write(-1, "x", 1);
    printf("libc err ret: %zd errno: %d\n", r1, errno);

    errno = 0;
    r2 = ft_write(-1, "x", 1);
    printf("asm  err ret: %zd errno: %d\n", r2, errno);
}

void test_read()
{
    printf("\n=== read ===\n");

    char buf1[10];
    char buf2[10];

    int fd = open("Makefile", O_RDONLY);

    ssize_t r1 = read(fd, buf1, 5);
    ssize_t r2 = ft_read(fd, buf2, 5);

    buf1[r1] = 0;
    buf2[r2] = 0;

    printf("libc: %s (%zd)\n", buf1, r1);
    printf("asm : %s (%zd)\n", buf2, r2);

    close(fd);
}

void test_strdup()
{
    printf("\n=== strdup ===\n");

    char *s1 = strdup("hello");
    char *s2 = ft_strdup("hello");

    printf("libc: %s\n", s1);
    printf("asm : %s\n", s2);

    free(s1);
    free(s2);
}

int main()
{
    test_strlen();
    test_strcpy();
    test_strcmp();
    test_write();
    test_read();
    test_strdup();

    return 0;
}
