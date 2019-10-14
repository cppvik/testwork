#include <stdio.h>
#include <pcap.h>

void close_handles(pcap_t *p, pcap_dumper_t *pd, struct bpf_program *fp)
{
    if (fp->bf_len != 0)
    {
        pcap_freecode(fp);
    }
    if (pd)
    {
        pcap_dump_close(pd);
    }
    if (p)
    {
        pcap_close(p);
    }
    return;
}

int main()
{
    pcap_t *p = NULL;
    pcap_dumper_t *pd = NULL;
    char error_buffer[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    fp.bf_len = 0;
    char filter[] = "not port 35782";
    char fname[] = "dump1.pcap";
    char dname[] = "dump2.pcap";
    int n;

    p = pcap_open_offline(fname, error_buffer);
    if (!p)
    {
        printf("Failed to open %s: %s\n", fname, error_buffer);
        close_handles(p, pd, &fp);
        return 1;
    }

    pd = pcap_dump_open(p, dname);
    if (!pd)
    {
        pcap_perror(p, "Failed to open dump file");
        close_handles(p, pd, &fp);
        return 2;
    }

    if (pcap_compile(p, &fp, filter, 0, PCAP_NETMASK_UNKNOWN))
    {
        pcap_perror(p, "Failed compiling filter");
        close_handles(p, pd, &fp);
        return 3;
    }
    if (pcap_setfilter(p, &fp))
    {
        pcap_perror(p, "Failed setting filter");
        close_handles(p, pd, &fp);
        return 4;
    }

    n = pcap_dispatch(p, 0, &pcap_dump, (char *) pd);
    if (n == -1 || n == -2)
    {
        pcap_perror(p, "Failed dispatching packets");
        close_handles(p, pd, &fp);
        return 5;
    }

    close_handles(p, pd, &fp);
    return 0;
}