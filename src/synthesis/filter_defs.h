/*
** Shortcircuit XT is Free and Open Source Software
**
** Shortcircuit is made available under the Gnu General Public License, v3.0
** https://www.gnu.org/licenses/gpl-3.0.en.html; The authors of the code
** reserve the right to re-license their contributions under the MIT license in the
** future at the discretion of the project maintainers.
**
** Copyright 2004-2022 by various individuals as described by the git transaction log
**
** All source at: https://github.com/surge-synthesizer/shortcircuit-xt.git
**
** Shortcircuit was a commercial product from 2004-2018, with copyright and ownership
** in that period held by Claes Johanson at Vember Audio. Claes made Shortcircuit
** open source in December 2020.
*/

#pragma once
#include "filter.h"
#include "morphEQ.h"
#include "parameterids.h"
#include "resampling.h"
#include "sampler_state.h"
#include "synthesis/biquadunit.h"
#include <memory>
#include <vt_dsp/basic_dsp.h>
#include <vt_dsp/lattice.h>
#include <vt_dsp/lipol.h>
#include "sst/filters/HalfRateFilter.h"

//-------------------------------------------------------------------------------------------------------

const char str_freqdef[] = ("f,-5,0.04,6,5,Hz"), str_freqmoddef[] = ("f,-12,0.04,12,0,oct"),
           str_timedef[] = ("f,-10,0.1,10,4,s"), str_lfofreqdef[] = ("f,-5,0.04,6,4,Hz"),
           str_percentdef[] = ("f,0,0.005,1,1,%"), str_percentbpdef[] = ("f,-1,0.005,1,1,%"),
           str_percentmoddef[] = ("f,-32,0.005,32,1,%"), str_dbdef[] = ("f,-96,0.1,12,0,dB"),
           str_dbbpdef[] = ("f,-48,0.1,48,0,dB"), str_dbmoddef[] = ("f,-96,0.1,96,0,dB"),
           str_mpitch[] = ("f,-96,0.04,96,0,cents"), str_bwdef[] = ("f,0.001,0.005,6,0,oct");

const int tail_infinite = 0x1000000;

//-------------------------------------------------------------------------------------------------------

class alignas(16) LP2A : public filter
{
    biquadunit lp;

  public:
    LP2A(float *);
    void process(float *data, float pitch);
    virtual void init_params();
    virtual void suspend() { lp.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) superbiquad : public filter
{
    biquadunit bq alignas(16)[4];

  public:
    superbiquad(float *, int *, int);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend();
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);
    virtual int tail_length() { return tail_infinite; }

  protected:
    int initmode;
    // lattice_sd d;
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) SuperSVF : public filter
{
  private:
    __m128 Freq, dFreq, Q, dQ, MD, dMD, ClipDamp, dClipDamp, Gain, dGain, Reg[3], LastOutput;

    sst::filters::HalfRate::HalfRateFilter mPolyphase;

    inline __m128 process_internal(__m128 x, int Mode);

  public:
    SuperSVF(float *, int *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);

    template <bool Stereo, bool FourPole>
    void ProcessT(float *datainL, float *datainR, float *dataoutL, float *dataoutR, float pitch);

    virtual void init_params();
    virtual void suspend();
    void calc_coeffs();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);
    virtual int tail_length() { return tail_infinite; }
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) LP2B : public filter
{
    biquadunit lp;

  public:
    LP2B(float *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend() { lp.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) LP4M_sat : public filter
{
    lipol_ps gain;
    sst::filters::HalfRate::HalfRateFilter pre_filter, post_filter;
    float reg alignas(16)[10];

  public:
    LP4M_sat(float *, int *);
    virtual ~LP4M_sat();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);
    virtual int tail_length() { return tail_infinite; }

  protected:
    bool first_run;
    lag<double> g, r;
};

//-------------------------------------------------------------------------------------------------------

/*	HP2A				*/

class alignas(16) HP2A : public filter
{
    biquadunit hp;

  public:
    HP2A(float *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend() { hp.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

/*	BP2A				*/

class alignas(16) BP2A : public filter
{
    biquadunit bq;

  public:
    BP2A(float *);
    void process(float *data, float pitch);
    virtual void init_params();
    virtual void suspend() { bq.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) BP2B : public filter
{
    biquadunit bq;

  public:
    BP2B(float *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend() { bq.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) BP2AD : public filter
{
    biquadunit bq alignas(16)[2];

  public:
    BP2AD(float *);
    virtual ~BP2AD();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend()
    {
        bq[0].suspend();
        bq[1].suspend();
    }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
    void calcfreq(float *a, float *b);
};

//-------------------------------------------------------------------------------------------------------

/*	PKA				*/

class alignas(16) PKA : public filter
{
    biquadunit bq;

  public:
    PKA(float *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend() { bq.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) PKAD : public filter
{
    biquadunit bq alignas(16)[2];

  public:
    PKAD(float *);
    virtual ~PKAD();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend()
    {
        bq[0].suspend();
        bq[1].suspend();
    }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
    void calcfreq(float *a, float *b);
};

//-------------------------------------------------------------------------------------------------------

/* LP+HP serial	*/

class alignas(16) LPHP_par : public filter
{
    biquadunit bq alignas(16)[2];

  public:
    LPHP_par(float *);
    virtual ~LPHP_par();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend()
    {
        bq[0].suspend();
        bq[1].suspend();
    }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) LPHP_ser : public filter
{
    biquadunit bq alignas(16)[2];

  public:
    LPHP_ser(float *);
    virtual ~LPHP_ser();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend()
    {
        bq[0].suspend();
        bq[1].suspend();
    }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
};

//-------------------------------------------------------------------------------------------------------

/*	NOTCH				*/

class alignas(16) NOTCH : public filter
{
  protected:
    biquadunit bq;

  public:
    NOTCH(float *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend() { bq.suspend(); }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);
};

//-------------------------------------------------------------------------------------------------------

/*	EQ2BP - 2 band parametric EQ				*/

class alignas(16) EQ2BP_A : public filter
{
  protected:
    biquadunit parametric alignas(16)[2];

  public:
    EQ2BP_A(float *, int *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend()
    {
        parametric[0].suspend();
        parametric[1].suspend();
    }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);
};

//-------------------------------------------------------------------------------------------------------

/*	EQ6B - 6 band graphic EQ				*/

class alignas(16) EQ6B : public filter
{
  protected:
    biquadunit parametric alignas(16)[6];

  public:
    EQ6B(float *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    void calc_coeffs();
    virtual void suspend()
    {
        parametric[0].suspend();
        parametric[1].suspend();
        parametric[2].suspend();
        parametric[3].suspend();
        parametric[4].suspend();
        parametric[5].suspend();
    }
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    } // filter z-plot honk (show per waveformdisplay)
    virtual float get_freq_graph(float f);
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) morphEQ : public filter
{
    lipol_ps gain;
    biquadunit b alignas(16)[8];

  public:
    morphEQ(float *, void *, int *);

    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);

    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    } // filter z-plot honk (show per waveformdisplay)
    virtual float get_freq_graph(float f);
    // virtual int get_entry_count(int p_id);
    // virtual const char*	get_entry_label(int p_id, int c_id);
    virtual void suspend()
    {
        b[0].suspend();
        b[1].suspend();
        b[2].suspend();
        b[3].suspend();
        b[4].suspend();
        b[5].suspend();
        b[6].suspend();
        b[7].suspend();
    }

  protected:
    bool b_active[8];
    float gaintarget;
    meq_snapshot snap[2];
    void *loader;
};

//-------------------------------------------------------------------------------------------------------

/*	EQ2BP - 2 band parametric EQ				*/

class alignas(16) LP2HP2_morph : public filter
{
  protected:
    biquadunit f;

  public:
    LP2HP2_morph(float *);
    void process(float *data, float pitch);
    virtual void init_params();
    void calc_coeffs();
    virtual void suspend() { f.suspend(); }
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    } // filter z-plot honk (show per waveformdisplay)
    virtual float get_freq_graph(float f);
};

//-------------------------------------------------------------------------------------------------------

/* comb filter	*/

const int comb_max_delay = 8192;

class alignas(16) COMB1 : public filter
{
  public:
    COMB1(float *);
    virtual ~COMB1();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int tail_length() { return tail_infinite; }

  protected:
    float delayloop[2][comb_max_delay];
    lipol<float> delaytime, feedback;
    int wpos;
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) COMB3 : public filter
{
  public:
    COMB3(float *);
    virtual ~COMB3();
    void process(float *data, float pitch);
    virtual void init_params();
    virtual int tail_length() { return tail_infinite; }

  protected:
    float delayloop[comb_max_delay];
    lipol<float> delaytime, feedback;
    int wpos;
};

//-------------------------------------------------------------------------------------------------------

/*	comb filter 2			*/

class alignas(16) COMB2 : public filter
{
  public:
    COMB2(float *);
    void process(float *data, float pitch);
    virtual void init_params();
    virtual void suspend()
    {
        f.suspend();
        feedback = 0;
    }
    virtual int tail_length() { return tail_infinite; }

  protected:
    biquadunit f;
    double feedback;
    lag<double> fbval;
};

//-------------------------------------------------------------------------------------------------------

/*	BF				*/

class alignas(16) BF : public filter
{
  public:
    BF(float *);
    virtual ~BF();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();

  protected:
    float time[2], level[2], postslew[2], lp_params[6];
    LP2B *lp;
};

//-------------------------------------------------------------------------------------------------------

/*	OD				*/

class alignas(16) OD : public filter
{
  public:
    OD(float *);
    virtual ~OD();
    void process(float *data, float pitch);
    virtual void init_params();

  protected:
    float time, level, postslew, lp_params[6], pk_params[6];
    std::unique_ptr<filter> lp2a, peak;
};

//-------------------------------------------------------------------------------------------------------

/*	treemonster				*/

class alignas(16) treemonster : public filter
{
    biquadunit locut;
    quadr_osc osc alignas(16)[2];
    lipol_ps gain alignas(16)[2];

  public:
    treemonster(float *, int *);
    virtual ~treemonster();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);

  protected:
    float length[2];
    float lastval[2];
};

//-------------------------------------------------------------------------------------------------------

/*	clipper				*/

class alignas(16) clipper : public filter
{
  public:
    lipol_ps pregain, postgain;
    clipper(float *);
    virtual ~clipper();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();

  protected:
};

//-------------------------------------------------------------------------------------------------------

/*	stereotools				*/

class alignas(16) stereotools : public filter
{
  public:
    lipol_ps ampL, ampR, width;
    stereotools(float *, int *);
    virtual ~stereotools();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) limiter : public filter
{
  protected:
    // Align16 biquadunit bq;
  public:
    limiter(float *, int *);
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend();
    virtual void init();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);

  protected:
    lipol_ps pregain, postgain;
    float ef, at, re;
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) fdistortion : public filter
{
  public:
    lipol_ps gain;
    sst::filters::HalfRate::HalfRateFilter pre, post;

    fdistortion(float *);
    virtual ~fdistortion();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    // void suspend();
    virtual void init_params();

  protected:
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) fslewer : public filter
{
    biquadunit bq alignas(16)[2];

  public:
    fslewer(float *);
    virtual ~fslewer();

    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);

    virtual void init_params();
    virtual void suspend()
    {
        bq[0].suspend();
        bq[1].suspend();
        v[0] = 0;
        v[1] = 0;
    }
    void calc_coeffs();
    virtual bool init_freq_graph()
    {
        calc_coeffs();
        return true;
    }
    virtual float get_freq_graph(float f);

  protected:
    lipol<float> rate;
    float v[2];
};
/*
class alignas(16)fexciter : public filter
{
public:
        fexciter(float*);
        virtual ~fexciter();
        void process(float *data, float pitch);
        //void suspend();
        virtual void init_params();
protected:
        lipol<float> amount;
        float reg;
        CHalfBandFilter<2> pre;
        CHalfBandFilter<4> post;
};*/

/*
bygg clipper med x times OS

class alignas(16)clipperHQ : public filter
{
public:
        clipper(float*);
        ~clipper();
        virtual void process(float *data, float pitch);
        virtual void init_params();
protected:
};*/

//-------------------------------------------------------------------------------------------------------

/*	gate				*/

class alignas(16) gate : public filter
{
  public:
    gate(float *);
    virtual ~gate();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();

  protected:
    int holdtime;
    bool gate_state, gate_zc_sync[2];
    float onesampledelay[2];
};

//-------------------------------------------------------------------------------------------------------

const int mg_bufsize = 4096;
class alignas(16) microgate : public filter
{
  public:
    microgate(float *);
    virtual ~microgate();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int tail_length() { return tail_infinite; }

  protected:
    int holdtime;
    bool gate_state, gate_zc_sync[2];
    float onesampledelay[2];
    float loopbuffer[2][mg_bufsize];
    int bufpos[2], buflength[2];
    bool is_recording[2];
};

//-------------------------------------------------------------------------------------------------------

/* RING					*/

class alignas(16) RING : public filter
{
    sst::filters::HalfRate::HalfRateFilter pre, post;
    quadr_osc qosc;

  public:
    RING(float *);
    virtual ~RING();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();

  protected:
    lipol<float, true> amount;
};

//-------------------------------------------------------------------------------------------------------

/* frequency shafter					*/

class alignas(16) FREQSHIFT : public filter
{
  protected:
    sst::filters::HalfRate::HalfRateFilter fcL, fcR;
    quadr_osc o1, o2;

  public:
    FREQSHIFT(float *, int *);
    virtual ~FREQSHIFT();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);
};

//-------------------------------------------------------------------------------------------------------

/* PMOD					*/

class alignas(16) PMOD : public filter
{
    lipol_ps pregain, postgain;
    sst::filters::HalfRate::HalfRateFilter pre, post;

  public:
    PMOD(float *);
    virtual ~PMOD();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();

  protected:
    double phase[2];
    lipol<float, true> omega;
};

//-------------------------------------------------------------------------------------------------------

/* oscillators			*/
/* pulse				*/
const int ob_length = 16;

class alignas(16) osc_pulse : public filter
{
    float oscbuffer alignas(16)[ob_length];

  public:
    osc_pulse(float *);
    virtual ~osc_pulse();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int tail_length() { return tail_infinite; }

  protected:
    void convolute();
    bool first_run;
    int64_t oscstate;
    float pitch;
    int polarity;
    float osc_out;
    int bufpos;
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) osc_pulse_sync : public filter
{
    float oscbuffer alignas(16)[ob_length];

  public:
    osc_pulse_sync(float *);
    virtual ~osc_pulse_sync();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int tail_length() { return tail_infinite; }

  protected:
    void convolute();
    bool first_run;
    int64_t oscstate, syncstate, lastpulselength;
    float pitch;
    int polarity;
    float osc_out;
    int bufpos;
};

//-------------------------------------------------------------------------------------------------------

/* saw	*/
const int max_unison = 16;

class alignas(16) osc_saw : public filter
{
    float oscbuffer alignas(16)[ob_length];

  public:
    osc_saw(float *, int *);
    virtual ~osc_saw();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);
    virtual int tail_length() { return tail_infinite; }

  protected:
    void convolute(int);
    bool first_run;
    int64_t oscstate[max_unison];
    float pitch;
    float osc_out;
    float out_attenuation;
    float detune_bias, detune_offset;
    float dc, dc_uni[max_unison];
    int bufpos;
    int n_unison;
};

//-------------------------------------------------------------------------------------------------------

/* sin	*/
class alignas(16) osc_sin : public filter
{
  public:
    osc_sin(float *);
    virtual ~osc_sin();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual int tail_length() { return tail_infinite; }

  protected:
    quadr_osc osc;
};

//-------------------------------------------------------------------------------------------------------

// new effects (from SURGE)

const int max_delay_length = 1 << 18;
const int slowrate = 8;
const int slowrate_m1 = slowrate - 1;

//-------------------------------------------------------------------------------------------------------

class alignas(16) dualdelay : public filter
{
    lipol_ps feedback, crossfeed, pan;
    float buffer alignas(16)[2][max_delay_length + FIRipol_N];

  public:
    dualdelay(float *, int *);
    virtual ~dualdelay();
    virtual void init();
    virtual void init_params();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void suspend();
    void setvars(bool init);

  private:
    lag<double, true> timeL, timeR;
    float envf;
    int wpos;
    biquadunit lp, hp;
    double lfophase;
    float LFOval;
    bool LFOdirection;
    int ringout_time;
};

//-------------------------------------------------------------------------------------------------------

/* rotary speaker	*/
class alignas(16) rotary_speaker : public filter
{
    biquadunit xover, lowbass;

  public:
    rotary_speaker(float *, int *);
    virtual ~rotary_speaker();
    // void process_only_control();
    virtual void suspend();
    void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void init();

  protected:
    float buffer[max_delay_length];
    int wpos;
    quadr_osc lfo;
    quadr_osc lf_lfo;
    lipol<float> dL, dR, drive, hornamp[2];
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) phaser : public filter
{
    float L alignas(16)[BLOCK_SIZE], R alignas(16)[BLOCK_SIZE];
    biquadunit *biquad alignas(16)[8];

  public:
    phaser(float *, int *);
    virtual ~phaser();
    // void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void init();
    virtual void suspend();
    void setvars();

  protected:
    lipol<float, true> feedback;
    static const int n_bq = 4;
    static const int n_bq_units = n_bq << 1;
    float dL, dR;
    __m128d ddL, ddR;
    float lfophase;
    int bi; // block increment (to keep track of events not occurring every n blocks)
};

//-------------------------------------------------------------------------------------------------------

/* SC V1 effects	*/

class alignas(16) fauxstereo : public filter
{
    lipol_ps l_amplitude, l_source;

  public:
    fauxstereo(float *, int *);
    virtual ~fauxstereo();
    // void process(float *datain, float *dataout, float pitch);
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    // virtual void init();
    // virtual void suspend();
  protected:
    // lag<float,true> l_amplitude,l_source;
    std::unique_ptr<COMB3> combfilter;
    float fp[n_filter_parameters];
};

//-------------------------------------------------------------------------------------------------------

/*	fs_flange			*/
class alignas(16) fs_flange : public filter
{
  public:
    fs_flange(float *, int *);
    virtual ~fs_flange();
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();

  protected:
    lipol<float> dry, wet, feedback;
    filter *freqshift[2];
    float f_fs[2][n_filter_parameters];
    int i_fs[2][n_filter_iparameters];
    float fs_buf[2][BLOCK_SIZE];
};

//-------------------------------------------------------------------------------------------------------

/*	freqshiftdelay				*/
class alignas(16) freqshiftdelay : public filter
{
  public:
    freqshiftdelay(float *, int *);
    virtual ~freqshiftdelay();
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void suspend();

  protected:
    float *buffer;
    int bufferlength;
    float delaytime_filtered;
    int wpos;
    filter *freqshift;
    float f_fs[n_filter_parameters];
    int i_fs[n_filter_iparameters];
};

//-------------------------------------------------------------------------------------------------------

const int revbits = 15;
const int max_rev_dly = 1 << revbits;
const int rev_tap_bits = 4;
const int rev_taps = 1 << rev_tap_bits;

class alignas(16) reverb : public filter
{
    float delay_pan_L alignas(16)[rev_taps], delay_pan_R alignas(16)[rev_taps];
    float delay_fb alignas(16)[rev_taps];
    float delay alignas(16)[rev_taps * max_rev_dly];
    float out_tap alignas(16)[rev_taps];
    float predelay alignas(16)[max_rev_dly];
    biquadunit band1, locut, hicut;
    int delay_time alignas(16)[rev_taps];
    lipol_ps width;

  public:
    reverb(float *, int *);
    virtual ~reverb();
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void init();
    virtual void suspend();
    virtual int get_ip_count();
    virtual const char *get_ip_label(int ip_id);
    virtual int get_ip_entry_count(int ip_id);
    virtual const char *get_ip_entry_label(int ip_id, int c_id);

  protected:
    void update_rtime();
    void update_rsize();
    void clear_buffers();
    void loadpreset(int id);
    int delay_pos;
    double modphase;
    int shape;
    float lastf[n_filter_parameters];
    int ringout_time;
    int b;
};

//-------------------------------------------------------------------------------------------------------

class alignas(16) chorus : public filter
{
    lipol_ps feedback, width;
    __m128 voicepanL4 alignas(16)[4], voicepanR4 alignas(16)[4];
    biquadunit lp, hp;
    float buffer alignas(16)[max_delay_length + FIRipol_N];
    // s� kan den interpoleras med SSE utan wrap
  public:
    chorus(float *, int *);
    virtual ~chorus();
    void process_stereo(float *datainL, float *datainR, float *dataoutL, float *dataoutR,
                        float pitch);
    virtual void init_params();
    virtual void init();
    virtual void suspend();

  protected:
    void setvars(bool init);
    lag<float, true> time[4];
    float voicepan[4][2];
    float envf;
    int wpos;
    double lfophase[4];
};
