/*
 * Copyright (C) 2013  Pekka Enberg
 *
 * This file is released under the GPL version 2 with the following
 * clarification and special exception:
 *
 *     Linking this library statically or dynamically with other modules is
 *     making a combined work based on this library. Thus, the terms and
 *     conditions of the GNU General Public License cover the whole
 *     combination.
 *
 *     As a special exception, the copyright holders of this library give you
 *     permission to link this library with independent modules to produce an
 *     executable, regardless of the license terms of these independent
 *     modules, and to copy and distribute the resulting executable under terms
 *     of your choice, provided that you also meet, for each linked independent
 *     module, the terms and conditions of the license of that module. An
 *     independent module is a module which is not derived from or based on
 *     this library. If you modify this library, you may extend this exception
 *     to your version of the library, but you are not obligated to do so. If
 *     you do not wish to do so, delete this exception statement from your
 *     version.
 *
 * Please refer to the file LICENSE for details.
 */

#include "dalvik/classloader.h"

#include "lib/options.h"

#include "vm/system.h"
#include "vm/utf8.h"

#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool			running_on_valgrind;

static char		*program_name;
static const char	*classpath;
static const char	*class_name;
static unsigned int	nr_class_args;
static char		**class_args;

static void die(const char *format, ...)
{
	va_list ap;

	fprintf(stderr, "%s: ", program_name);

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fprintf(stderr, "\n");

	exit(EXIT_FAILURE);
}

#define USAGE_TEXT									\
	"\n"										\
	"%s: [options] class [argument ...]\n"						\
	"\n"										\
	"The following standard options are recognized:\n"				\
	"  -classpath classpath\n"							\
	"  -help\n"									\

static void usage(void)
{
	fprintf(stderr, USAGE_TEXT, program_name);

	exit(EXIT_FAILURE);
}

static void opt_help(void)
{
	fprintf(stdout, USAGE_TEXT, program_name);

	exit(EXIT_SUCCESS);
}

static void opt_classpath(const char *arg)
{
	classpath = arg;
}

const struct option options[] = {
	DEFINE_OPTION	 ("help", opt_help),

	DEFINE_OPTION_ARG("classpath", opt_classpath),
};

static void parse_options(int argc, char *argv[])
{
	int optind;

	for (optind = 1; optind < argc; ++optind) {
		const struct option *opt;

		if (argv[optind][0] != '-')
			break;

		opt = get_option(options, ARRAY_SIZE(options), argv[optind] + 1);
		if (!opt)
			die("Unrecognized option '%s'", argv[optind]);

		if (!opt->arg) {
			opt->handler.func();
			continue;
		}

		if (opt->arg_is_adjacent) {
			opt->handler.func_arg(argv[optind] + strlen(opt->name) + 1);
			continue;
		}

		/* We wanted an argument, but there was none */
		if (optind + 1 >= argc)
			usage();

		opt->handler.func_arg(argv[++optind]);
	}

	if (optind >= argc)
		usage();

	class_name = dots_to_slash(argv[optind++]);

	if (optind < argc) {
		nr_class_args = argc - optind;
		class_args = &argv[optind];
	}
}

int main(int argc, char *argv[])
{
	struct dalvik_classloader *loader;
	struct vm_class *vmc;

	program_name = basename(argv[0]);

	parse_options(argc, argv);

	loader = dalvik_classloader_new(classpath);
	if (!loader)
		die("Could not instantiate classloader for classpath '%s'", classpath);

	vmc = dalvik_class_load(loader, class_name);
	if (!vmc)
		die("Cound not find class '%s'", class_name);

	return 0;
}
