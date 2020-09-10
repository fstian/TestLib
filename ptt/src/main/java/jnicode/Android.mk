LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libcodeutils

LOCAL_SRC_FILES := com_witted_ptt_CodeUtils.c \
					acelp_ca.c \
					basicop.c \
					bits.c \
					calcexc.c \
					cod_ld8a.c \
					cor_func.c\
					de_acelp.c \
					dec_gain.c \
					dec_lag3.c \
					dec_ld8a.c \
					dec_sid.c \
					dspfunc.c \
					dtx.c \
					enc_sid.c \
					filter.c \
					g729ab.c \
					gainpred.c \
					lpc.c \
					lpcfunc.c \
					lspdec.c \
					lspgetq.c \
					p_parity.c \
					pitch_a.c \
					post_pro.c \
					postfilt.c \
					pre_proc.c \
					pred_lt3.c \
					qsidgain.c \
					qsidlsf.c \
					qua_gain.c \
					qua_lsp.c \
					tab_dtx.c \
					tab_ld8a.c \
					taming.c \
					util.c \
                    vad.c


include $(BUILD_SHARED_LIBRARY)