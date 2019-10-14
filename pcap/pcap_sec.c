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

void secmark_packet_handler(
    u_char *user,
    const struct pcap_pkthdr *header,
    const u_char *packet
)
{
    u_char level = 0;
    const u_char *ip_header;
    int ip_header_length;

    ip_header = packet + 14 /* ethernet header length */;
    /* The IHL is number of 32-bit segments. Multiply
       by four to get a byte count for pointer arithmetic */
    ip_header_length = ((*ip_header) & 0x0F) * 4;

    if (ip_header_length >= 24)
    {
        u_char sec_type = *(ip_header + 20);
        u_char sec_length = *(ip_header + 21) - 3;
        u_char class = *(ip_header + 22);
        if (sec_type == 0x82 && class == 0xAB && sec_length >= 1)
        {
            level = *(ip_header + 23) >> 1;
            printf("SECMARK detected: level %d\n", level);
        }
    }
    if (level <= 1)
    {
        pcap_dump(user, header, packet);
    }

    return;
}

int main()
{
    pcap_t *p;
    pcap_dumper_t *pd = NULL;
    char error_buffer[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    fp.bf_len = 0;
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

    n = pcap_dispatch(p, 0, &secmark_packet_handler, (char *) pd);
    if (n == -1 || n == -2)
    {
        pcap_perror(p, "Failed dispatching packets");
        close_handles(p, pd, &fp);
        return 5;
    }

    close_handles(p, pd, &fp);
    return 0;
}