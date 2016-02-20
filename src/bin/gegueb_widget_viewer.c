#include "Gegueb.h"
#include <gtk/gtk.h>

static void destroy (GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

int main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *w;
	Enesim_Stream *s;
	Egueb_Dom_Node *doc = NULL;

	gegueb_init();
	s = enesim_stream_file_new(argv[1], "rb");
	if (!s)
	{
		printf("Failed to read file %s\n", argv[1]);
		return -1;
	}
	egueb_dom_parser_parse(s, &doc);
	if (!doc)
	{
		printf("Failed to parse file %s\n", argv[1]);
		return -1;
	}

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Gegueb Viewer");

	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(destroy), NULL);
	w = gegueb_widget_new();
	g_object_set(w, "document", doc, NULL);
	gtk_container_add(GTK_CONTAINER (window), w);
	gtk_widget_show(w);
	gtk_widget_show(window);

	gtk_main ();
	return 0;
}
