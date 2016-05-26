
/*
 * Hacked up postinst program to install the default score file. We have to do it this
 * way, since the old score file is overwritten if the score file already exists in the
 * package - abz
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* location of score file */
static const char filename[] = "/var/games/tint.scores";

/* user name of default score file */
static const char user[] = "root";

/* group name of default score file */
static const char group[] = "games";

/* contents of default score file */
static const uint8_t contents[] =
{
   0x54, 0x69, 0x6e, 0x74, 0x20, 0x30, 0x2e, 0x30,
   0x32, 0x62, 0x20, 0x28, 0x63, 0x29, 0x20, 0x41,
   0x62, 0x72, 0x61, 0x68, 0x61, 0x6d, 0x20, 0x76,
   0x64, 0x20, 0x4d, 0x65, 0x72, 0x77, 0x65, 0x20,
   0x2d, 0x20, 0x53, 0x63, 0x6f, 0x72, 0x65, 0x73,
   0x52, 0x69, 0x61, 0x61, 0x6e, 0x20, 0x45, 0x6e,
   0x67, 0x65, 0x6c, 0x62, 0x72, 0x65, 0x63, 0x68,
   0x74, 0x00, 0x10, 0x5f, 0x00, 0x00, 0xb7, 0xc8,
   0x20, 0x3c, 0x41, 0x62, 0x7a, 0x00, 0x80, 0x3a,
   0x00, 0x00, 0x94, 0xc5, 0x20, 0x3c, 0x52, 0x69,
   0x61, 0x61, 0x6e, 0x20, 0x45, 0x6e, 0x67, 0x65,
   0x6c, 0x62, 0x72, 0x65, 0x63, 0x68, 0x74, 0x00,
   0x66, 0x21, 0x00, 0x00, 0x31, 0xc9, 0x20, 0x3c,
   0x52, 0x69, 0x61, 0x61, 0x6e, 0x20, 0x45, 0x6e,
   0x67, 0x65, 0x6c, 0x62, 0x72, 0x65, 0x63, 0x68,
   0x74, 0x00, 0xdc, 0x1a, 0x00, 0x00, 0x79, 0xc6,
   0x20, 0x3c, 0x52, 0x69, 0x61, 0x61, 0x6e, 0x20,
   0x45, 0x6e, 0x67, 0x65, 0x6c, 0x62, 0x72, 0x65,
   0x63, 0x68, 0x74, 0x00, 0x2f, 0x16, 0x00, 0x00,
   0xf9, 0xc5, 0x20, 0x3c, 0x52, 0x69, 0x61, 0x61,
   0x6e, 0x20, 0x45, 0x6e, 0x67, 0x65, 0x6c, 0x62,
   0x72, 0x65, 0x63, 0x68, 0x74, 0x00, 0x8b, 0x11,
   0x00, 0x00, 0x82, 0xc7, 0x20, 0x3c, 0x41, 0x62,
   0x7a, 0x00, 0x23, 0x10, 0x00, 0x00, 0xa9, 0xc9,
   0x20, 0x3c, 0x44, 0x6f, 0x70, 0x70, 0x65, 0x6c,
   0x67, 0x61, 0x6e, 0x67, 0x65, 0x72, 0x00, 0x13,
   0x0b, 0x00, 0x00, 0x21, 0xc7, 0x20, 0x3c, 0x4a,
   0x6f, 0x68, 0x61, 0x6e, 0x6e, 0x20, 0x42, 0x6f,
   0x74, 0x68, 0x61, 0x00, 0x5a, 0x09, 0x00, 0x00,
   0xef, 0xc6, 0x20, 0x3c, 0x41, 0x62, 0x7a, 0x00,
   0xd7, 0x07, 0x00, 0x00, 0xad, 0xc6, 0x20, 0x3c
};

static void debhelper_stuff()
{
   /* dh_installdocs */
   system ("if [ -d /usr/doc -a ! -e /usr/doc/tint -a -d /usr/share/doc/tint ]; then\n"
		   "	ln -sf ../share/doc/tint /usr/doc/tint\n"
		   "fi\n");

   /* dh_installmenu */
   system ("if [ -x /usr/bin/update-menus ]; then update-menus ; fi");
}

int main (int argc,char *argv[])
{
   struct stat sb;

   /* we only do something if we're called as <program> configure ... */
   if (argc < 2 || strcmp (argv[1],"configure")) exit (EXIT_SUCCESS);

   /* if the score file doesn't exist, create it */
   if (stat (filename,&sb) < 0)
	 {
		int fd,result;
		struct passwd *u;
		struct group *g;
		uid_t uid;
		gid_t gid;

		/* get uid */
		if ((u = getpwnam (user)) == NULL)
		  {
			 fprintf (stderr,"Couldn't obtain uid for %s: %m\n",user);
			 exit (EXIT_FAILURE);
		  }
		uid = u->pw_uid;

		/* get gid */
		if ((g = getgrnam (group)) == NULL)
		  {
			 fprintf (stderr,"Couldn't obtain gid for %s: %m\n",group);
			 exit (EXIT_FAILURE);
		  }
		gid = g->gr_gid;

		/* create default score file */
		if ((fd = creat (filename,0664)) < 0)
		  {
			 fprintf (stderr,"Couldn't create score file %s: %m\n",filename);
			 exit (EXIT_FAILURE);
		  }
		result = write (fd,contents,sizeof (contents));
		if (result < 0)
		  {
			 fprintf (stderr,"Unable to write to %s: %m\n",filename);
			 close (fd);
			 unlink (filename);
			 exit (EXIT_FAILURE);
		  }
		if (result < sizeof (contents))
		  {
			 fprintf (stderr,"Short write count. %d/%d bytes written to %s\n",result,sizeof (contents),filename);
			 close (fd);
			 unlink (filename);
			 exit (EXIT_FAILURE);
		  }
		close (fd);

		/* change ownership of score file */
		if (chown (filename,uid,gid) < 0)
		  {
			 fprintf (stderr,"Couldn't change ownership of %s to %s:%s: %m\n",filename,user,group);
			 unlink (filename);
			 exit (EXIT_FAILURE);
		  }
	 }

   debhelper_stuff ();

   exit (EXIT_SUCCESS);
}

