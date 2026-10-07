#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

GtkWidget *entry_ip;
GtkWidget *entry_cidr;
GtkWidget *label_result;

void calculate_ip(const char *ip_str, int cidr) {
    unsigned int b1,b2,b3,b4;
    if (sscanf(ip_str, "%u.%u.%u.%u", &b1,&b2,&b3,&b4)!= 4) {
        gtk_label_set_text(GTK_LABEL(label_result), "IP eka waradi! Ex: 192.168.1.1");
        return;
    }
    if (cidr < 0 || cidr > 32) {
        gtk_label_set_text(GTK_LABEL(label_result), "CIDR eka 0-32 athara danna!");
        return;
    }
    uint32_t ip = (b1<<24)|(b2<<16)|(b3<<8)|b4;
    uint32_t mask = cidr==0? 0 : (0xFFFFFFFF << (32-cidr));
    uint32_t network = ip & mask;
    uint32_t broadcast = network | (~mask);

    unsigned int n1 = (network>>24)&0xFF, n2=(network>>16)&0xFF, n3=(network>>8)&0xFF, n4=network&0xFF;
    unsigned int bc1 = (broadcast>>24)&0xFF, bc2=(broadcast>>16)&0xFF, bc3=(broadcast>>8)&0xFF, bc4=broadcast&0xFF;
    unsigned int m1 = (mask>>24)&0xFF, m2=(mask>>16)&0xFF, m3=(mask>>8)&0xFF, m4=mask&0xFF;

    uint32_t hosts = cidr >=31? 0 : (broadcast - network -1);
    if (cidr==32) hosts=1;
    if (cidr==31) hosts=2;

    uint32_t first = hosts>0? network+1 : network;
    uint32_t last = hosts>0? broadcast-1 : broadcast;
    if (cidr>=31) { first=network; last=broadcast; }

    unsigned int f1=(first>>24)&0xFF, f2=(first>>16)&0xFF, f3=(first>>8)&0xFF, f4=first&0xFF;
    unsigned int l1=(last>>24)&0xFF, l2=(last>>16)&0xFF, l3=(last>>8)&0xFF, l4=last&0xFF;

    char result[1024];
    snprintf(result, sizeof(result),
        "IP: %s/%d\nSubnet Mask: %u.%u.%u.%u\nNetwork: %u.%u.%u.%u\nBroadcast: %u.%u.%u.%u\nFirst Usable: %u.%u.%u.%u\nLast Usable: %u.%u.%u.%u\nTotal Hosts: %u",
        ip_str, cidr, m1,m2,m3,m4, n1,n2,n3,n4, bc1,bc2,bc3,bc4, f1,f2,f3,f4, l1,l2,l3,l4, hosts
    );
    gtk_label_set_text(GTK_LABEL(label_result), result);
}

void on_calculate_clicked(GtkButton *button, gpointer user_data) {
    const char *ip_text = gtk_editable_get_text(GTK_EDITABLE(entry_ip));
    const char *cidr_text = gtk_editable_get_text(GTK_EDITABLE(entry_cidr));
    if (strlen(ip_text)==0 || strlen(cidr_text)==0) {
        gtk_label_set_text(GTK_LABEL(label_result), "IP saha CIDR deka danna!");
        return;
    }
    int cidr = atoi(cidr_text);
    calculate_ip(ip_text, cidr);
}

void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MS Network Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *title = gtk_label_new("##MS Network##\nIP Calculator");
    gtk_box_append(GTK_BOX(box), title);

    entry_ip = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_ip), "Ex: 192.168.20.8");
    gtk_editable_set_text(GTK_EDITABLE(entry_ip), "192.168.20.8");
    gtk_box_append(GTK_BOX(box), entry_ip);

    entry_cidr = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_cidr), "Ex: 25");
    gtk_editable_set_text(GTK_EDITABLE(entry_cidr), "25");
    gtk_box_append(GTK_BOX(box), entry_cidr);

    GtkWidget *btn = gtk_button_new_with_label("CALCULATE");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_calculate_clicked), NULL);
    gtk_box_append(GTK_BOX(box), btn);

    label_result = gtk_label_new("Result eka methana enawa...");
    gtk_label_set_wrap(GTK_LABEL(label_result), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label_result), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_result), 0.0);
    gtk_box_append(GTK_BOX(box), label_result);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.ms.network", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}