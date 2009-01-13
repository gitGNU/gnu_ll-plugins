/*
    Copyright (C) 2006-2007 Fons Adriaensen <fons@kokkinizita.net>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "zita-convolver.h"




Convproc::Convproc (void) :
    _state (ST_IDLE),
    _flags (0),
    _ninp (0),
    _nout (0),
    _nproc (0),
    _fftwopt (FFTW_ESTIMATE)
{
    memset (_inpbuff, 0, MAXINP * sizeof (float *));
    memset (_outbuff, 0, MAXOUT * sizeof (float *));
}


Convproc::~Convproc (void)
{
    cleanup ();
}


int Convproc::configure (unsigned int ninp,
                         unsigned int nout,
                         unsigned int maxsize,
                         unsigned int quantum,
                         unsigned int minpart,
			 unsigned int maxpart)
{
    unsigned int  i, j, k, m, n, r, s;

    if (_state != ST_IDLE) return Converror::BAD_STATE;
    if (   (quantum & (quantum - 1))
        || (quantum < MINQUANT)
        || (quantum > MAXQUANT)
        || (minpart & (minpart - 1))
	|| (minpart < MINPART)
        || (minpart < quantum)
        || (minpart > MAXDIVIS * quantum)
        || (maxpart & (maxpart - 1))
	|| (maxpart > MAXPART)
	|| (maxpart < minpart)) return Converror::BAD_PARAM;

    cleanup ();

    s = minpart;
    if (s & PMASK2)
    {
	m = 2;
	r = 2;
    }
    else if (s & PMASK4)
    {
	m = 6;
	r = 4;
    }
    else return Converror::BAD_PARAM;
    if (minpart == quantum) m++;

    try
    {
	for (i = j = 0; i < maxsize; j++)
	{
	    k = (maxsize - i + s - 1) / s;
	    if (s < maxpart)
	    {
		n = (ninp * nout < 16) ? m : m + 2;
		if (k > n) k = m;
	    }
	    _procs [j].configure (i, k, s, minpart, _fftwopt);
	    i += s * k;
	    if (i < maxsize)
	    {
		s *= r;
		r = 4;
		m = 6;
	    }
	}	

	_ninp = ninp;
	_nout = nout;
	_nproc = j;
	_quantum = quantum;
	_minpart = minpart;
	_maxpart = s;
	_inpsize = 2 * s;
	 
	for (k = 0; k < ninp; k++)
	{
	    _inpbuff [k] = new float [_inpsize];
	    memset (_inpbuff [k], 0, _inpsize * sizeof (float));
	}
	for (k = 0; k < nout; k++)
	{
	    _outbuff [k] = new float [_minpart];
	    memset (_outbuff [k], 0, _minpart * sizeof (float));
	}
    }
    catch (...)
    {
	cleanup ();
	return Converror::MEM_ALLOC;
    }

    _state = ST_STOP;
    return 0;
}


int Convproc::impdata_create (unsigned int inp,
                              unsigned int out,
                              unsigned int step,
                              float       *data,
                              int          ind0,
                              int          ind1)
{
    unsigned int j;

    if (_state != ST_STOP) return Converror::BAD_STATE;
    try
    {
        for (j = 0; j < _nproc; j++)
	{
            _procs [j].impdata_create (inp, out, step, data, ind0, ind1);
	}
    }
    catch (...)
    {
	cleanup ();
	return Converror::MEM_ALLOC;
    }
    return 0;
}


int Convproc::impdata_update (unsigned int inp,
                              unsigned int out,
                              unsigned int step,
                              float       *data, 
                              int          ind0,
                              int          ind1)
{
    unsigned int j;

    if (_state < ST_STOP) return Converror::BAD_STATE;
    for (j = 0; j < _nproc; j++)
    {
        _procs [j].impdata_update (inp, out, step, data, ind0, ind1);
    }
    return 0;
}


int Convproc::impdata_copy (unsigned int inp1,
                            unsigned int out1,
                            unsigned int inp2,
                            unsigned int out2) 
{
    unsigned int j;

    if (_state != ST_STOP) return Converror::BAD_STATE;
    try
    {
        for (j = 0; j < _nproc; j++)
	{
            _procs [j].impdata_copy (inp1, out1, inp2, out2);
	}
    }
    catch (...)
    {
	cleanup ();
	return Converror::MEM_ALLOC;
    }

    return 0;
}


int Convproc::start_process (int maxprio)
{
    unsigned int j, k;

    if (_state != ST_STOP) return Converror::BAD_STATE;

    for (k = 0; k < _nproc; k++) _procs [k].prepare (_inpsize, _inpbuff);
    j = (_minpart == _quantum) ? 1 : 0;
    for (k = j; k < _nproc; k++) _procs [k].start (maxprio - 1 + j - k);

    _flags = 0;
    _inpoffs = 0;
    _outoffs = 0;
    _procdel = 0;
    _state = ST_PROC;

    return 0;
}


int Convproc::stop_process (void)
{
    unsigned int k;

    if (_state != ST_PROC) return Converror::BAD_STATE;

    _state = ST_WAIT;
    for (k = 0; k < _nproc; k++) _procs [k].stop ();

    return 0;
}


int Convproc::cleanup (void)
{
    unsigned int k;

    if (_state != ST_STOP) return Converror::BAD_STATE;

    for (k = 0; k < _ninp; k++) delete[] _inpbuff [k];
    for (k = 0; k < _nout; k++) delete[] _outbuff [k];
    for (k = 0; k < _nproc; k++) _procs [k].cleanup ();
    memset (_inpbuff, 0, MAXINP * sizeof (float *));
    memset (_outbuff, 0, MAXOUT * sizeof (float *));

    _ninp = 0;
    _nout = 0;
    _nproc = 0;
    _minpart = 0;
    _inpsize = 0;
    _state = ST_IDLE;

    return 0;
}


void Convproc::process (void)
{
    unsigned int f, k;

    if (_state != ST_PROC) return;
    
    _inpoffs += _quantum;
    if (_inpoffs == _inpsize) _inpoffs = 0;

    _outoffs += _quantum;
    if (_outoffs == _minpart)
    {
        _outoffs = f = 0;
	for (k = 0; k < _nout; k++) memset (_outbuff [k], 0, _minpart * sizeof (float));
	for (k = 0; k < _nproc; k++) f |= _procs [k].readout (_outbuff);
        if (f)
	{
	    _flags |= f;
            if (++_procdel >= 3)
            {
	        stop_process ();
	        _flags = FL_LOAD;
	    }
	}
        else _procdel = 0;
    }
}


void Convproc::check (void)
{
    unsigned int k;

    for (k = 0; (k < _nproc) && _procs [k].idle (); k++);
    if (k == _nproc) _state = ST_STOP;
}


void Convproc::print (void)
{
    unsigned int k;

    for (k = 0; k < _nproc; k++) _procs [k].print ();
}




Convlevel::Convlevel (void) :
    _stat (ST_IDLE),
    _npar (0),
    _parsize (0),
    _pthr (0),
    _inp_list (0),
    _out_list (0),
    _plan_r2c (0),
    _plan_c2r (0),
    _time_data (0),
    _prep_data (0),
    _freq_data (0)
{
}


Convlevel::~Convlevel (void)
{
    cleanup ();
}


void *Convlevel::alloc_aligned (size_t size)
{
    void *p;

    if (posix_memalign (&p, 16, size)) throw (Converror (Converror::MEM_ALLOC));
    memset (p, 0, size);
    return p;
}


void Convlevel::configure (unsigned int offs,
                           unsigned int npar,
                           unsigned int parsize,
                           unsigned int outstep,
			   unsigned int fftwopt)
{
    _offs = offs;
    _npar = npar;
    _norm = 0.5f / parsize;
    _parsize = parsize;
    _outstep = outstep;
    
    _time_data = (float *)(alloc_aligned (2 * _parsize * sizeof (float)));
    _prep_data = (float *)(alloc_aligned (2 * _parsize * sizeof (float)));
    _freq_data = (fftwf_complex *)(alloc_aligned ((_parsize + 1) * sizeof (fftwf_complex)));
    _plan_r2c = fftwf_plan_dft_r2c_1d (2 * _parsize, _time_data, _freq_data, fftwopt);
    _plan_c2r = fftwf_plan_dft_c2r_1d (2 * _parsize, _freq_data, _time_data, fftwopt);
    if (_plan_r2c && _plan_c2r) return;
    throw (Converror (Converror::MEM_ALLOC));
}


void Convlevel::impdata_create (unsigned int inp,
                                unsigned int out,
                                unsigned int step,
                                float *data,
                                int i0,
                                int i1)
{
    unsigned int   k;
    int            j, j0, j1, n;
    fftwf_complex  *fftb;
    Macnode        *M;

    n = i1 - i0;
    i0 = _offs - i0;
    i1 = i0 + _npar * _parsize;
    if ((i0 >= n) || (i1 <= 0)) return;

    M = findmacnode (inp, out, true);
    if (! (M->_fftb))
    {
	M->_fftb = new fftwf_complex * [_npar];
	memset (M->_fftb, 0, _npar * sizeof (fftwf_complex *));
    }

    for (k = 0; k < _npar; k++)
    {
	i1 = i0 + _parsize;
	if ((i0 < n) && (i1 > 0))
	{
	    if (! (M->_fftb [k]))
	    {
		M->_fftb [k] = (fftwf_complex *)(alloc_aligned ((_parsize + 1) * sizeof (fftwf_complex)));
	    }
	    memset (_prep_data, 0, 2 * _parsize * sizeof (float));
	    j0 = (i0 < 0) ? 0 : i0;
	    j1 = (i1 > n) ? n : i1;
	    for (j = j0; j < j1; j++) _prep_data [j - i0] = _norm * data [j * step];
	    fftwf_execute_dft_r2c (_plan_r2c, _prep_data, _freq_data);
  	    fftb = M->_fftb [k];
	    for (j = 0; j <= (int)_parsize; j++)
	    {
	        fftb [j][0] += _freq_data [j][0];
	        fftb [j][1] += _freq_data [j][1];
	    }
	}
	i0 = i1;
    }
}


void Convlevel::impdata_update (unsigned int inp,
                                unsigned int out,
                                unsigned int step,
                                float *data,
                                int i0,
                                int i1)
{
    unsigned int   k;
    int            j, j0, j1, n;
    fftwf_complex  *fftb;
    Macnode        *M;

    M = findmacnode (inp, out, false);
    if (! M) return;

    n = i1 - i0;
    i0 = _offs - i0;
    i1 = i0 + _npar * _parsize;
    if ((i0 >= n) || (i1 <= 0)) return;

    for (k = 0; k < _npar; k++)
    {
	i1 = i0 + _parsize;
	fftb = M->_fftb [k];
	if (fftb && (i0 < n) && (i1 > 0))
	{
	    memset (_prep_data, 0, 2 * _parsize * sizeof (float));
	    j0 = (i0 < 0) ? 0 : i0;
	    j1 = (i1 > n) ? n : i1;
	    for (j = j0; j < j1; j++) _prep_data [j - i0] = _norm * data [j * step];
	    fftwf_execute_dft_r2c (_plan_r2c, _prep_data, fftb);
	}
	i0 = i1;
    }
}


void Convlevel::impdata_copy (unsigned int inp1,
                              unsigned int out1,
                              unsigned int inp2,
                              unsigned int out2)
{
    Macnode  *M1;
    Macnode  *M2;

    M1 = findmacnode (inp1, out1, false);
    if (! M1) return;
    M2 = findmacnode (inp2, out2, true);
    if (M2->_fftb) return;
    M2->_fftb = M1->_fftb;
    M2->_copy = true;
}


void Convlevel::prepare (unsigned int inpsize, float **_inpbuff)
{
    Inpnode   *X;

    _inpsize = inpsize;
    if (_parsize == _outstep)
    {
        _outoffs = 0;
        _inpoffs = 0;
    }
    else
    {
        _outoffs = _parsize / 2;
        _inpoffs = _inpsize - _outoffs;
    }

    _bits = _parsize / _outstep;
    _late = 0;
    _ipar = 0;
    _opi1 = 0;
    _opi2 = 1;
    _opi3 = 2;
    sem_init (&_trig, 0, 0);

    for (X = _inp_list; X; X = X->_next)
    {
	X->_buff = _inpbuff [X->_inp];
    }
}


void Convlevel::start (int prio)
{
    int                min, max;
    pthread_attr_t     attr;
    struct sched_param parm;

    _pthr = 0;
    min = sched_get_priority_min (SCHED_FIFO);
    max = sched_get_priority_max (SCHED_FIFO);
    if (prio > max) prio = max;
    if (prio < min) prio = min;
    parm.sched_priority = prio;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
    pthread_attr_setschedparam (&attr, &parm);
    pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setstacksize (&attr, 0x10000);
    pthread_create (&_pthr, &attr, static_main, this);
    pthread_attr_destroy (&attr);
}


void Convlevel::stop (void)
{
    if (_stat != ST_IDLE)
    {
        _stat = ST_TERM;
	sem_post (&_trig);
    }
}


void Convlevel::cleanup (void)
{
    unsigned int  i;
    Inpnode       *X, *X1;
    Outnode       *Y, *Y1;
    Macnode       *M, *M1;

    X = _inp_list;
    while (X)
    {
        for (i = 0; i < _npar; i++) free (X->_ffta [i]);
	delete[] X->_ffta;
	X1 = X->_next;
	delete X;
	X = X1;
    }
    _inp_list = 0;

    Y = _out_list;
    while (Y)
    {
	M = Y->_list;
	while (M)
	{
	    if ((M->_fftb) && !(M->_copy))
	    {
	        for (i = 0; i < _npar; i++) 
		{
                    free (M->_fftb [i]);
		}
	        delete[] M->_fftb;
	    }
	    M1 = M->_next;
	    delete M;
	    M = M1;
	}
	for (i = 0; i < 3; i++) free (Y->_buff [i]);
	Y1 = Y->_next;
	delete Y;
	Y = Y1;
    }
    _out_list = 0;

    fftwf_destroy_plan (_plan_r2c);
    fftwf_destroy_plan (_plan_c2r);
    free (_time_data);
    free (_prep_data);
    free (_freq_data);
    _plan_r2c = 0;
    _plan_c2r = 0;
    _time_data = 0;
    _prep_data = 0;
    _freq_data = 0;
}


void *Convlevel::static_main (void *arg)
{
    ((Convlevel *) arg)->main ();
    return 0;
}


void Convlevel::main (void)
{
    _stat = ST_PROC;
    while (true)
    {
	sem_wait (&_trig);
	if (_stat == ST_TERM)
	{
            _stat = ST_IDLE;
	    _pthr = 0;
            return;
        }
	process ();
    }
}


void Convlevel::process (void)
{
    unsigned int    i, j, k;
    unsigned int    i1, n1, n2;

    Inpnode         *X;
    Macnode         *M;
    Outnode         *Y;
    fftwf_complex   *ffta;
    fftwf_complex   *fftb;
    float           *outd;

    i1 = _inpoffs;
    n1 = _parsize;
    n2 = 0;
    _inpoffs = i1 + n1;
    if (_inpoffs >= _inpsize)
    {
        _inpoffs -= _inpsize;
	n2 = _inpoffs;
	n1 -= n2;
    }

    for (X = _inp_list; X; X = X->_next)
    {
	memcpy (_time_data, X->_buff + i1, n1 * sizeof (float));
	if (n2) memcpy (_time_data + n1, X->_buff, n2 * sizeof (float));
	memset (_time_data + _parsize, 0, _parsize * sizeof (float));
	fftwf_execute_dft_r2c (_plan_r2c, _time_data, X->_ffta [_ipar]);
    }

    for (Y = _out_list; Y; Y = Y->_next)
    {
        memset (_freq_data, 0, (_parsize + 1) * sizeof (fftwf_complex));
	for (M = Y->_list; M; M = M->_next)
	{
	    X = M->_inpn;
            i = _ipar;
	    for (j = 0; j < _npar; j++)
	    {
		ffta = X->_ffta [i];
		fftb = M->_fftb [j];
		if (fftb)
		{
		    for (k = 0; k <= _parsize; k++)
		    {
			_freq_data [k][0] += ffta [k][0] * fftb [k][0] - ffta [k][1] * fftb [k][1];
			_freq_data [k][1] += ffta [k][0] * fftb [k][1] + ffta [k][1] * fftb [k][0];
		    }
		}
		if (i == 0) i = _npar;
		i--;
	    }
	}

	fftwf_execute_dft_c2r (_plan_c2r, _freq_data, _time_data);
	outd = Y->_buff [_opi2];
	for (k = 0; k < _parsize; k++) outd [k] += _time_data [k];
	outd = Y->_buff [_opi3];
	memcpy (outd, _time_data + _parsize, _parsize * sizeof (float));
    }

    _ipar++;
    if (_ipar == _npar) _ipar = 0;
}


int Convlevel::readout (float **outbuff)
{
    unsigned int    k;
    Outnode        *Y;
    float           *p, *q;	

    _outoffs += _outstep;
    if (_outoffs == _parsize)
    {
	_outoffs = 0;
	if (_stat == ST_PROC)
	{
	    sem_getvalue (&_trig, &_late);
            sem_post (&_trig);
	}
        else process ();
        k = _opi1;
	_opi1 = _opi2;
	_opi2 = _opi3;
	_opi3 = k;
    }

    for (Y = _out_list; Y; Y = Y->_next)
    {
        p = Y->_buff [_opi1] + _outoffs;
	q = outbuff [Y->_out];
	for (k = 0; k < _outstep; k++) q [k] += p [k];
    }

    return _late ? _bits : 0;
}


void Convlevel::print (void)
{
    printf ("offs = %6d,  parsize = %5d,  npar = %3d\n", _offs, _parsize, _npar);
}


Macnode *Convlevel::findmacnode (unsigned int inp, unsigned int out, bool create)
{
    unsigned int  i;
    Inpnode       *X;
    Outnode       *Y;
    Macnode       *M;

    for (X = _inp_list; X && (X->_inp != inp); X = X->_next);
    if (! X)
    {
	if (! create) return 0;
	X = new Inpnode;
	X->_next = _inp_list;
	_inp_list = X;
	X->_inp = inp;
	X->_buff = 0;
	X->_ffta = new fftwf_complex * [_npar];
	memset (X->_ffta, 0, _npar * sizeof (fftw_complex *));
        for (i = 0; i < _npar; i++)
	{
            X->_ffta [i] = (fftwf_complex *)(alloc_aligned ((_parsize + 1) * sizeof (fftwf_complex)));
	}
    }

    for (Y = _out_list; Y && (Y->_out != out); Y = Y->_next);
    if (! Y)
    {
	if (! create) return 0;
	Y = new Outnode;
	Y->_next = _out_list;
	_out_list = Y;
	Y->_out = out;
	Y->_list = 0;
        for (i = 0; i < 3; i++)
	{
	    Y->_buff [i] = 0;
	}
        for (i = 0; i < 3; i++)
	{
	    Y->_buff [i] = (float *)(alloc_aligned (_parsize * sizeof (float)));
	}
    }

    for (M = Y->_list; M && (M->_inpn != X); M = M->_next);
    if (! M)
    {
	if (! create) return 0;
	M = new Macnode;
	M->_next = Y->_list;
	Y->_list = M;
	M->_inpn = X;
	M->_fftb = 0;
	M->_copy = false;
    }

    return M;
}




