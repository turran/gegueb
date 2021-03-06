#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include "Gegueb.h"
#include <glib.h>

static GMainLoop *loop;

static void help(const char *name)
{
	printf("Usage: %s [OPTIONS] FILE\n", name);
	printf("Where OPTIONS can be one of the following:\n");
	printf("-h Print this screen\n");
	printf("-w The width of the window\n");
	printf("-e The height of the window\n");
	printf("And FILE can be a SVG file or a directory\n");
}

static void _gegueb_window_close_cb(Egueb_Dom_Event *e,
		void *data)
{
	g_main_loop_quit(loop);
}

int main(int argc, char *argv[])
{
	Egueb_Dom_Window *w;
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_Event_Target *et;
	Enesim_Stream *s;
	Eina_Bool free_file = EINA_FALSE;
	char *short_options = "hw:e:";
	struct option long_options[] = {
		{ "help", 1, 0, 'h' },
		{ "width", 1, 0, 'w' },
		{ "height", 1, 0, 'e' },
	};
	int option;
	int ret;
	char *filename;
	int width;
	int height;
	int fps;
	struct stat st;

	/* default options */
	width = -1;
	height = -1;
	fps = 30;

	if (!gegueb_init())
		goto gegueb_failed;

	while ((ret = getopt_long(argc, argv, short_options, long_options,
			&option)) != -1)
	{
		switch (ret)
		{
			case 'h':
			help(argv[0]);
			return 0;

			case 'w':
			width = atoi(optarg);
			break;

			case 'e':
			height = atoi(optarg);
			break;

			default:
			break;
		}
	}

	if (argc - 1 != optind)
	{
		help(argv[0]);
		return 0;
	}

	filename = argv[optind];
	if (stat(filename, &st) < 0)
	{
		printf("No file found\n");
		help(argv[0]);
		goto done;
	}
	/* sanitize the filename */
	if (*filename != '.' && *filename != '/')
	{
		char wd[PATH_MAX];

		if (!getcwd(wd, PATH_MAX))
			goto done;
		if (asprintf(&filename, "%s/%s", wd, filename) < 0)
			goto done;
		free_file = EINA_TRUE;
	}

	s = enesim_stream_file_new(filename, "rb");
	if (!s)
	{
		printf("Failed to read file %s\n", filename);
		help(argv[0]);
		goto read_error;
	}
	egueb_dom_parser_parse(s, &doc);
	w = gegueb_window_new(doc, 0, 0, width, height);
	if (!w)
		goto shutdown_gegueb;

	et = EGUEB_DOM_EVENT_TARGET(w);
	egueb_dom_event_target_event_listener_add(et,
			EGUEB_DOM_EVENT_WINDOW_CLOSE,
			_gegueb_window_close_cb,
			EINA_TRUE, NULL);
	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	egueb_dom_window_unref(w);
	gegueb_shutdown();

	return 0;

shutdown_gegueb:
	gegueb_shutdown();
gegueb_failed:
read_error:
	if (free_file)
		free(filename);
done:
	return -1;
}

