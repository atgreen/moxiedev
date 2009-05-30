/* This file was generated automatically by genconf.c. */
#ifdef device_
device_(gs_pbm_device)
device_(gs_pbmraw_device)
device_(gs_pgm_device)
device_(gs_pgmraw_device)
device_(gs_pgnm_device)
device_(gs_pgnmraw_device)
device_(gs_pnm_device)
device_(gs_pnmraw_device)
device_(gs_ppm_device)
device_(gs_ppmraw_device)
device_(gs_pdfwrite_device)
device_(gs_pswrite_device)
device_(gs_epswrite_device)
device_(gs_pxlmono_device)
device_(gs_pxlcolor_device)
device_(gs_nullpage_device)
#endif
#ifdef oper_
oper_(zchar1_op_defs)
oper_(zfont1_op_defs)
oper_(zmisc1_op_defs)
#endif
#ifdef psfile_
psfile_("gs_type1.ps")
#endif
#ifdef oper_
oper_(zvmem2_op_defs)
oper_(zdps1_l2_op_defs)
#endif
#ifdef psfile_
psfile_("gs_dps1.ps")
#endif
#ifdef oper_
oper_(zmisc2_op_defs)
oper_(zusparam_op_defs)
#endif
#ifdef psfile_
psfile_("gs_lev2.ps")
psfile_("gs_res.ps")
#endif
#ifdef oper_
oper_(zchar42_op_defs)
oper_(zfont42_op_defs)
#endif
#ifdef psfile_
psfile_("gs_typ42.ps")
psfile_("gs_cidfn.ps")
#endif
#ifdef oper_
oper_(zcid_op_defs)
oper_(zcie_l2_op_defs)
oper_(zcrd_l2_op_defs)
oper_(zfcmap_op_defs)
#endif
#ifdef psfile_
psfile_("gs_cmap.ps")
#endif
#ifdef oper_
oper_(zfont0_op_defs)
oper_(zchar2_op_defs)
oper_(zfdcte_op_defs)
oper_(zfdctd_op_defs)
oper_(zdevice2_l2_op_defs)
oper_(ziodev2_l2_op_defs)
oper_(zmedia2_l2_op_defs)
#endif
#ifdef io_device_
io_device_(gs_iodev_null)
io_device_(gs_iodev_ram)
io_device_(gs_iodev_calendar)
#endif
#ifdef psfile_
psfile_("gs_setpd.ps")
#endif
#ifdef oper_
oper_(zbseq_l2_op_defs)
#endif
#ifdef psfile_
psfile_("gs_btokn.ps")
#endif
#ifdef init_
init_(gs_gscolor1_init)
init_(gs_gximage3_init)
#endif
#ifdef oper_
oper_(zcolor1_op_defs)
oper_(zht1_op_defs)
oper_(zupath_l2_op_defs)
oper_(ireclaim_l2_op_defs)
oper_(zchar2_l2_op_defs)
#endif
#ifdef psfile_
psfile_("gs_dps2.ps")
#endif
#ifdef oper_
oper_(zfdecode_op_defs)
oper_(zfilter2_op_defs)
oper_(zarith_op_defs)
oper_(zarray_op_defs)
oper_(zcontrol_op_defs)
oper_(zdict_op_defs)
oper_(zfile_op_defs)
oper_(zfileio_op_defs)
oper_(zfilter_op_defs)
oper_(zfproc_op_defs)
oper_(zgeneric_op_defs)
oper_(ziodev_op_defs)
oper_(zmath_op_defs)
oper_(zmisc_op_defs)
oper_(zpacked_op_defs)
oper_(zrelbit_op_defs)
oper_(zstack_op_defs)
oper_(zstring_op_defs)
oper_(zsysvm_op_defs)
oper_(ztoken_op_defs)
oper_(ztype_op_defs)
oper_(zvmem_op_defs)
oper_(zchar_op_defs)
oper_(zcolor_op_defs)
oper_(zdevice_op_defs)
oper_(zfont_op_defs)
oper_(zfont2_op_defs)
oper_(zgstate_op_defs)
oper_(zht_op_defs)
oper_(zimage_op_defs)
oper_(zmatrix_op_defs)
oper_(zpaint_op_defs)
oper_(zpath_op_defs)
#endif
#ifdef io_device_
io_device_(gs_iodev_stdin)
io_device_(gs_iodev_stdout)
io_device_(gs_iodev_stderr)
io_device_(gs_iodev_lineedit)
io_device_(gs_iodev_statementedit)
#endif
#ifdef init_
init_(gs_igcref_init)
#endif
#ifdef oper_
oper_(zfbcp_op_defs)
oper_(zhsb_op_defs)
oper_(zpath1_op_defs)
#endif
#ifdef init_
init_(gs_gstype1_init)
#endif
#ifdef emulator_
emulator_("PostScript")
emulator_("PostScriptLevel1")
#endif
#ifdef oper_
oper_(zpcolor_l2_op_defs)
#endif
#ifdef init_
init_(gs_gximage4_init)
init_(gs_gximage5_init)
#endif
#ifdef oper_
oper_(zcolor2_l2_op_defs)
oper_(zcsindex_l2_op_defs)
oper_(zht2_l2_op_defs)
oper_(zimage2_l2_op_defs)
oper_(zcssepr_l2_op_defs)
oper_(zfilterx_op_defs)
#endif
#ifdef emulator_
emulator_("PostScriptLevel2")
#endif
#ifdef psfile_
psfile_("gs_mex_e.ps")
psfile_("gs_mro_e.ps")
psfile_("gs_pdf_e.ps")
psfile_("gs_wan_e.ps")
#endif
#ifdef oper_
oper_(zfzlib_op_defs)
#endif
#ifdef psfile_
psfile_("gs_pdf.ps")
psfile_("gs_l2img.ps")
psfile_("pdf_base.ps")
psfile_("pdf_draw.ps")
psfile_("pdf_font.ps")
psfile_("pdf_main.ps")
psfile_("pdf_sec.ps")
psfile_("pdf_2ps.ps")
#endif
#ifdef emulator_
emulator_("PDF")
#endif
#ifdef psfile_
psfile_("gs_cff.ps")
psfile_("gs_ttf.ps")
#endif
#ifdef init_
init_(gs_gstype2_init)
#endif
#ifdef io_device_
io_device_(gs_iodev_pipe)
#endif
#ifdef init_
init_(gs_climag_init)
init_(gs_clpath_init)
#endif
#ifdef psfile_
psfile_("gs_pdfwr.ps")
#endif
#ifdef init_
init_(gs_gscolor_init)
init_(gs_gximage1_init)
init_(gs_gximage2_init)
init_(gs_roplib_init)
#endif
#define GS_LIB_DEFAULT "../share/ghostscript/5.0:../share/ghostscript/fonts"
#define SEARCH_HERE_FIRST 1
#define GS_DOCDIR "../share/ghostscript/5.0/doc"
#define GS_INIT "gs_init.ps"
#define GS_REVISION 500
#define GS_REVISIONDATE 19970606

