#ifndef __ARCH_HH__
#define __ARCH_HH__

module(Host, "Host");
CForwardChannel PCIE_Egress_ch ("PCIE_Egress_ch");
//PCIE_Egress_ch.SetWriterCount(2);
//PCIE_Egress_ch.SetReaderCount(2);

CForwardChannel PCIE_Ingress_ch ("PCIE_Ingress_ch");

CForwardChannel Host_mem_ch ("Host_mem_ch");


module(DMA_WRC, "DMA_WRC");

module(DMA_ODC1, "DMA_ODC1");
CForwardChannel ODC1_req_ch ("ODC1_req_ch", 1024); //Hao: 1K result buffer
//CAssembleChannel ODC1_req_ch ("ODC1_req_ch", 1024); //Hao: 1K result buffer
CForwardChannel ODC1_wr_ch ("ODC1_wr_ch");

module(DMA_ODC2, "DMA_ODC2");
CForwardChannel ODC2_req_ch ("ODC2_req_ch", 1024); //Hao: 1K result buffer
//CAssembleChannel ODC2_req_ch ("ODC2_req_ch", 1024); //Hao: 1K result buffer
CForwardChannel ODC2_wr_ch ("ODC2_wr_ch");

module(DMA_ODC3, "DMA_ODC3");
CForwardChannel ODC3_req_ch ("ODC3_req_ch", 1024); //Hao: 1K result buffer
//CAssembleChannel ODC3_req_ch ("ODC3_req_ch", 1024); //Hao: 1K result buffer
CForwardChannel ODC3_wr_ch ("ODC3_wr_ch");

module(DMA_ODC4, "DMA_ODC4");
CForwardChannel ODC4_req_ch ("ODC4_req_ch", 1024); //Hao: 1K result buffer
//CAssembleChannel ODC4_req_ch ("ODC4_req_ch", 1024); //Hao: 1K result buffer
CForwardChannel ODC4_wr_ch ("ODC4_wr_ch");

module(DMA_CPLS,"DMA_CPLS");
//Hao: add the module CPLS
CForwardChannel CPLS_req_ch("CPLS_req_ch", 1024); //Hao: 16B * 64(cmd) = 1024B descriptor buffer
CForwardChannel CPLS_wr_ch("CPLS_wr_ch");
//Hao: add two channels for CPLS

module(DMA_RRC, "DMA_RRC");
module(DMA_CC, "DMA_CC");


module(DMA_CPP, "DMA_CPP");
CForwardChannel CMD_req_ch ("CMD_req_ch");
CForwardChannel CMD_fetch_ch ("CMD_fetch_ch");
CForwardChannel CMD_cpl_ch ("CMD_cpl_ch");
CForwardChannel CMD_fwd_ch ("CMD_fwd_ch");
CForwardChannel CPR_req_ch ("CPR_req_ch"); //Hao: add this channel for CPR
CForwardChannel CPR_fwd_ch ("CPR_fwd_ch"); //Hao: add this channel for CPR


module(DMA_IDC1, "DMA_IDC1");
CForwardChannel IDC1_req_ch ("IDC1_req_ch");
CForwardChannel IDC1_cpl_ch ("IDC1_cpl_ch",4096); //Hao: 4K source buffer
CForwardChannel IDC1_out_ch ("IDC1_out_ch");
CForwardChannel IDC1_in_ch ("IDC1_in_ch");

module(DMA_IDC2, "DMA_IDC2");
CForwardChannel IDC2_req_ch ("IDC2_req_ch");
CForwardChannel IDC2_cpl_ch ("IDC2_cpl_ch",4096); //Hao: 4K source buffer
CForwardChannel IDC2_out_ch ("IDC2_out_ch");
CForwardChannel IDC2_in_ch ("IDC2_in_ch");

module(DMA_IDC3, "DMA_IDC3");
CForwardChannel IDC3_req_ch ("IDC3_req_ch");
CForwardChannel IDC3_cpl_ch ("IDC3_cpl_ch",4096); //Hao: 4K source buffer
CForwardChannel IDC3_out_ch ("IDC3_out_ch");
CForwardChannel IDC3_in_ch ("IDC3_in_ch");

module(DMA_IDC4, "DMA_IDC4");
CForwardChannel IDC4_req_ch ("IDC4_req_ch");
CForwardChannel IDC4_cpl_ch ("IDC4_cpl_ch",4096); //Hao: 4K source buffer
CForwardChannel IDC4_out_ch ("IDC4_out_ch");
CForwardChannel IDC4_in_ch ("IDC4_in_ch");


//module(XBAR_REQ, "XBAR_REQ");
//Hao: remove the module, the open point is reserved for CDMGR

module(XBAR_CPL, "XBAR_CPL");
module(XBAR_WR, "XBAR_WR");
//CAssembleChannel Mem_Data_ch ("Mem_Data_ch");
//Hao: remove above channel, the open point is reserved for EUx

//module(DS_WR, "DS_WR");
//module(DS_RD, "DS_RD");
//CForwardChannel mem_data_wr_ch ("mem_data_wr_ch");
//CForwardChannel mem_data_rd_ch ("mem_data_rd_ch");
//Hao: Remove the declaration for EUx

module (CMD_TRANS, "CMD_TRANS");
//Hao: add the CMD_TRANS for the convert from "command point" completion to "command" read request

//----------------------------------------
//Hao: open point modules
//----------------------------------------
module(XBAR_REQ, "XBAR_REQ");

module(EUx,"EUx");
CForwardChannel Mem_Data_in_ch ("Mem_Data_in_ch");
CForwardChannel Mem_Data_out_ch ("Mem_Data_out_ch");
CForwardChannel Mem_CTL_ch ("Mem_CTL_ch");

/* CAssembleChannel Mem_Data_in_ch ("Mem_Data_in_ch"); */
/* CAssembleChannel Mem_Data_out_ch ("Mem_Data_out_ch"); */
/* CAssembleChannel Mem_CTL_ch ("Mem_CTL_ch"); */
//Hao:end



#endif
