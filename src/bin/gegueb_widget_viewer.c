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

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Gegueb Viewer");

	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(destroy), NULL);
	w = gegueb_widget_new(NULL);
	gtk_container_add(GTK_CONTAINER (window), w);
	gtk_drawing_area_size(GTK_DRAWING_AREA (w), 200, 200);
	gtk_widget_show(w);
	gtk_widget_show(window);

	gtk_main ();
	return 0;
}
