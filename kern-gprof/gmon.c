/*-
 * Copyright (c) 1983, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys/param.h>
#include <sys/time.h>
#include <sys/gmon.h>
#include <sys/gmon_out.h>
#include <sys/uio.h>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#ifdef USE_IN_LIBIO
# include <wchar.h>
#endif

#define attribute_hidden
static int	s_scale;
#define		SCALE_1_TO_1	0x10000L
#define __snprintf snprintf
#define __getpid getpid
#define __strerror_r strerror_r
#define __fxprintf(io,args...) fprintf(stderr,args)
#define __set_errno(x)
#define __setitimer setitimer

#define write_not_cancel write
#define open_not_cancel open
#define close_not_cancel_no_status close
#define writev_not_cancel_no_status writev

struct __bb *__bb_head attribute_hidden;
struct gmonparam _gmonparam ;/*attribute_hidden = { GMON_PROF_OFF };*/
 int  m=0;
/*
 * See profil(2) where this is described:
 */


#define ERR(s) write_not_cancel (STDERR_FILENO, s, sizeof (s) - 1)


static u_short *samples;
//static u_long *samples;
static size_t nsamples;
static size_t pc_offset;
static u_int pc_scale;

static void write_call_graph (int fd) ;

/*
 * Control profiling
 *	profiling is what mcount checks to see if
 *	all the data structures are ready.
 */
void
__moncontrol (mode)
     int mode;
{
  struct gmonparam *p = &_gmonparam;

  /* Don't change the state if we ran into an error.  */
  if (p->state == GMON_PROF_ERROR)
    return;

  if (mode)
    {
      /* start */
      __profil((void *) p->kcount, p->kcountsize, p->lowpc, s_scale);
      p->state = GMON_PROF_ON;
    }
  else
    {
      /* stop */
      __profil(NULL, 0, 0, 0);
      p->state = GMON_PROF_OFF;
    }
}
typedef int error_t;
error_t err;
void
__monstartup (u_long  lowpc, u_long  highpc)
{
  register int o=0;
  char *cp;
  struct gmonparam *p = &_gmonparam;
  
  /*
   * round lowpc and highpc to multiples of the density we're using
   * so the rest of the scaling (here and in gprof) stays in ints.
   */
  p->lowpc = ROUNDDOWN(lowpc, HISTFRACTION * sizeof(HISTCOUNTER));
  p->highpc = ROUNDUP(highpc, HISTFRACTION * sizeof(HISTCOUNTER));
  p->textsize = p->highpc - p->lowpc;
  p->kcountsize = ROUNDUP(p->textsize / HISTFRACTION, sizeof(*p->froms));
  p->hashfraction = HASHFRACTION;
  p->log_hashfraction = -1;
  /* The following test must be kept in sync with the corresponding
     test in mcount.c.  */
  if ((HASHFRACTION & (HASHFRACTION - 1)) == 0) {
      /* if HASHFRACTION is a power of two, mcount can use shifting
	 instead of integer division.  Precompute shift amount. */
      p->log_hashfraction = ffs(p->hashfraction * sizeof(*p->froms)) - 1;
  }
  p->fromssize = p->textsize / HASHFRACTION;
  p->tolimit = p->textsize * ARCDENSITY / 100;
 // printf("%ld",p->tolimit);
  if (p->tolimit < MINARCS)
    p->tolimit = MINARCS;
  else if (p->tolimit > MAXARCS)
    p->tolimit = MAXARCS;
  p->tossize = p->tolimit * sizeof(struct tostruct);

  cp = calloc (p->kcountsize + p->fromssize + p->tossize, 1);
  if (! cp)
    {
      ERR("monstartup: out of memory\n");
      p->tos = NULL;
      p->state = GMON_PROF_ERROR;
      return;
    }
  p->tos = (struct tostruct *)cp;
  cp += p->tossize;
  p->kcount = (HISTCOUNTER *)cp;
  cp += p->kcountsize;
  p->froms = (ARCINDEX *)cp;

  p->tos[0].link = 0;
 // printf("%d",o);
  o = p->highpc - p->lowpc;
  if (p->kcountsize < (u_long) o)
    {
#ifndef hp300
      s_scale = ((float)p->kcountsize / o ) * SCALE_1_TO_1;
#else
      /* avoid floating point operations */
      int quot = o / p->kcountsize;

      if (quot >= 0x10000)
	s_scale = 1;
      else if (quot >= 0x100)
	s_scale = 0x10000 / quot;
      else if (o >= 0x800000)
	s_scale = 0x1000000 / (o / (p->kcountsize >> 8));
      else
	s_scale = 0x1000000 / ((o << 8) / p->kcountsize);
#endif
    } else
      s_scale = SCALE_1_TO_1;
      __moncontrol(1);
  
}
int
__profil (u_short *sample_buffer, size_t size, size_t offset, u_int scale)
{
  samples = sample_buffer;
  nsamples = size / sizeof *samples;
 // nsamples = size ;
  pc_offset = offset;
  pc_scale = scale;
 // printf("%d",scale);

}
static void
profil_count (void *pc,u_long time)
{
  size_t i = (pc - pc_offset - (void *) 0) / 2;
  //printf("%d\n",time);
  if (sizeof (unsigned long long int) > sizeof (size_t))
    i = (unsigned long long int) i * pc_scale / 65536;
  else
    i = i / 65536 * pc_scale + i % 65536 * pc_scale / 65536;
 
  if (i < nsamples)
   {
   
     //m++;
     // if(samples[i]>65500)
     if(pc==0xc105ad63)
     printf("%ld,%x\n",samples[i],pc);
         
     samples[i]+=time;
     
   }
}

static void
write_hist (int fd)
     
{
  u_char tag = GMON_TAG_TIME_HIST;
  struct gmon_hist_hdr thdr __attribute__ ((aligned (__alignof__ (char *))));

  if (_gmonparam.kcountsize > 0)
    {
      struct iovec iov[3] =
        {
	  { &tag, sizeof (tag) },
	  { &thdr, sizeof (struct gmon_hist_hdr) },
	  { _gmonparam.kcount, _gmonparam.kcountsize }
	};

      *(char **) thdr.low_pc = (char *) _gmonparam.lowpc;
      *(char **) thdr.high_pc = (char *) _gmonparam.highpc;
      *(int32_t *) thdr.hist_size = (_gmonparam.kcountsize
				     / sizeof (HISTCOUNTER));
      *(int32_t *) thdr.prof_rate = 100;
      strncpy (thdr.dimen, "seconds", sizeof (thdr.dimen));
      thdr.dimen_abbrev = 's';

      writev_not_cancel_no_status (fd, iov, 3);
    }
}

 void
write_call_graph (int fd)
{
#define NARCS_PER_WRITEV	32
  u_char tag = GMON_TAG_CG_ARC;
  struct gmon_cg_arc_record raw_arc[NARCS_PER_WRITEV]
    __attribute__ ((aligned (__alignof__ (char*))));
  ARCINDEX from_index, to_index;
  u_long from_len;
  u_long frompc;
  struct iovec iov[2 * NARCS_PER_WRITEV];
  int nfilled;

  for (nfilled = 0; nfilled < NARCS_PER_WRITEV; ++nfilled)
    {
      iov[2 * nfilled].iov_base = &tag;
      iov[2 * nfilled].iov_len = sizeof (tag);

      iov[2 * nfilled + 1].iov_base = &raw_arc[nfilled];
      iov[2 * nfilled + 1].iov_len = sizeof (struct gmon_cg_arc_record);
    }

  nfilled = 0;
  from_len = _gmonparam.fromssize / sizeof (*_gmonparam.froms);
  for (from_index = 0; from_index < from_len; ++from_index)
    {
      if (_gmonparam.froms[from_index] == 0)
	continue;

      frompc = _gmonparam.lowpc;
      frompc += (from_index * _gmonparam.hashfraction
		 * sizeof (*_gmonparam.froms));
      for (to_index = _gmonparam.froms[from_index];
	   to_index != 0;
	   to_index = _gmonparam.tos[to_index].link)
	{
	  struct arc
	    {
	      char *frompc;
	      char *selfpc;
	      int32_t count;
	    }
	  arc;

	  arc.frompc = (char *) frompc;
	  arc.selfpc = (char *) _gmonparam.tos[to_index].selfpc;
	  arc.count  = _gmonparam.tos[to_index].count;
	  memcpy (raw_arc + nfilled, &arc, sizeof (raw_arc [0]));

	  if (++nfilled == NARCS_PER_WRITEV)
	    {
	      writev_not_cancel_no_status (fd, iov, 2 * nfilled);
	      nfilled = 0;
	    }
	}
    }
  if (nfilled > 0)
    writev_not_cancel_no_status (fd, iov, 2 * nfilled);
}

static void
write_bb_counts (int fd)
     
{
  struct __bb *grp;
  u_char tag = GMON_TAG_BB_COUNT;
  size_t ncounts;
  size_t i;

  struct iovec bbhead[2] =
    {
      { &tag, sizeof (tag) },
      { &ncounts, sizeof (ncounts) }
    };
  struct iovec bbbody[8];
  size_t nfilled;

  for (i = 0; i < (sizeof (bbbody) / sizeof (bbbody[0])); i += 2)
    {
      bbbody[i].iov_len = sizeof (grp->addresses[0]);
      bbbody[i + 1].iov_len = sizeof (grp->counts[0]);
    }

  /* Write each group of basic-block info (all basic-blocks in a
     compilation unit form a single group). */

  for (grp = __bb_head; grp; grp = grp->next)
    {
      ncounts = grp->ncounts;
      writev_not_cancel_no_status (fd, bbhead, 2);
      for (nfilled = i = 0; i < ncounts; ++i)
	{
	  if (nfilled > (sizeof (bbbody) / sizeof (bbbody[0])) - 2)
	    {
	      writev_not_cancel_no_status (fd, bbbody, nfilled);
	      nfilled = 0;
	    }

	  bbbody[nfilled++].iov_base = (char *) &grp->addresses[i];
	  bbbody[nfilled++].iov_base = &grp->counts[i];
	}
      if (nfilled > 0)
	writev_not_cancel_no_status (fd, bbbody, nfilled);
    }
}



static void
write_gmon (void)
{
    struct gmon_hdr ghdr __attribute__ ((aligned (__alignof__ (int))));
    int fd = -1;
    char *env;

#ifndef O_NOFOLLOW
# define O_NOFOLLOW	0
#endif

    env = getenv ("GMON_OUT_PREFIX");
    if (env != NULL )
      {
	size_t len = strlen (env);
	char buf[len + 20];
	__snprintf (buf, sizeof (buf), "%s.%u", env, __getpid ());
	fd = open_not_cancel (buf, O_CREAT|O_TRUNC|O_WRONLY|O_NOFOLLOW, 0666);
      }

    if (fd == -1)
      {
	fd = open_not_cancel ("gmon.out", O_CREAT|O_TRUNC|O_WRONLY|O_NOFOLLOW,
			      0666);
	if (fd < 0)
	  {
	    char buf[300];
	    int errnum = errno;
	    __fxprintf (NULL, "_mcleanup: gmon.out: %s\n",
			__strerror_r (errnum, buf, sizeof buf));
	    return;
	  }
      }

    /* write gmon.out header: */
    memset (&ghdr, '\0', sizeof (struct gmon_hdr));
    memcpy (&ghdr.cookie[0], GMON_MAGIC, sizeof (ghdr.cookie));
    *(int32_t *) ghdr.version = GMON_VERSION;
    write_not_cancel (fd, &ghdr, sizeof (struct gmon_hdr));

    /* write PC histogram: */
    write_hist (fd);

    /* write call-graph: */
    write_call_graph (fd);

    /* write basic-block execution counts: */
    write_bb_counts (fd);

    close_not_cancel_no_status (fd);
}


void
__write_profiling (void)
{
  int save = _gmonparam.state;
  _gmonparam.state = GMON_PROF_OFF;
  if (save == GMON_PROF_ON)
    write_gmon ();
  _gmonparam.state = save;
}
#ifndef SHARED
/* This symbol isn't used anywhere in the DSO and it is not exported.
   This would normally mean it should be removed to get the same API
   in static libraries.  But since profiling is special in static libs
   anyway we keep it.  But not when building the DSO since some
   quality assurance tests will otherwise trigger.  */

#endif


void
main (void)
{
 
  char str1[30];
  unsigned long l1=0,l2=0;
  unsigned long long l3=0;
 // __monstartup (0xc0103100, 0xc04f5e5f);
   __monstartup (0xc1004100,0xc1644dff);
  FILE *fp=fopen("addr_result-1.txt","r");
  // FILE *fp=fopen("addr","r");
 
  //char delims[]=" ";
  char *result=NULL,*result1=NULL;
  char *p=NULL,*m=NULL;
  char *num[3];
  unsigned long n=1,i;
  int mm=0;
while(fgets(str1,sizeof(str1),fp)!=NULL)
  {
   
    i=0;
    p=str1;
    
    while((num[i]=strtok(p,","))!=NULL)
    {
         
          if(i==0)
           {
            
             sscanf(num[i],"%x",&l1);
            }
          if(i==1)
          {
            sscanf(num[1],"%x",&l2);
        }


          i++;
         
          p=NULL;
     }
  
   
    
   // sscanf(num[1],"%x",&l2);
    //printf("%s",num[1]);
    sscanf(num[2],"%ld",&l3);
     mcount(l1,l2);
    // mcount(0xc0619f68,0xc0128770);
    // mcount(0xc012878e,0xc0619f6d);
    // m++;
     l3=l3/2674;
   // l3=sqrt(l3);
    profil_count (l2,l3);
  // printf("%ld\n",l3);
     
  
  
}

   // printf("%d\n",m);
 
 
  if (_gmonparam.state != GMON_PROF_ERROR)
    write_gmon ();
}
