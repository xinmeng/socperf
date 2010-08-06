/* transaction handler in host */

/* ----------------------------------------
   Host: PCIE.CMD.fetch -> PCIE.CMD.cpl
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_pcie_cmd_fetch, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.CMD.fetch");

CSource Host_PCIE_CMD_fetch_pkt("Host-in: PCIE.CMD.fetch packet", 
				&PCIE_Egress_ch, is_pcie_cmd_fetch);
CNameTransform xf_rename_PCIE_CMD_fetch_2_PCIE_CMD_cpl ("PCIE.CMD.fetch to PCIE.CMD.cpl", "PCIE.CMD.cpl");
CSizeTransform xf_resize_PCIE_CMD_fetch_2_PCIE_CMD_cpl ("Set PCIE.CMD.cpl size", 56); //Hao:32bits system 
//CSizeTransform xf_resize_PCIE_CMD_fetch_2_PCIE_CMD_cpl ("Set PCIE.CMD.cpl size", 72); //Hao:64bits system 
     //Hao:resize the command completion
     //32bits system:16B + 16B + 16B * (0-3) + (20 ~ 24)B = (52 ~ 56)B + 16B * (0-3)
     //64bits system:16B + 32B + 32B * (0-2) + (20 ~ 24)B = (68 ~ 72)B + 32B * (0-2)
CGroupTransform xf_PCIE_CMD_fetch_2_PCIE_CMD_cpl("PCIE.CMD.fetch to PCIE.CMD.cpl", 1);
xf_PCIE_CMD_fetch_2_PCIE_CMD_cpl.AddTransform(&xf_rename_PCIE_CMD_fetch_2_PCIE_CMD_cpl);
xf_PCIE_CMD_fetch_2_PCIE_CMD_cpl.AddTransform(&xf_resize_PCIE_CMD_fetch_2_PCIE_CMD_cpl);


CDestination Host_PCIE_CMD_cpl_pkt ("Host-out: PCIE.CMD.cpl packet", &PCIE_Ingress_ch,
				    2, GBps, 8);
//Hao:change the bandwidth
Host_PCIE_CMD_cpl_pkt.SetPipeline(200);
//Hao:set the completion latency  100* 8ns = 800ns
CSimpleTH th_PCIE_CMD_fetch_2_PCIE_CMD_cpl ("Host: PCIE.CMD.fetch -> PCIE.CMD.cpl", 
					    &Host_PCIE_CMD_fetch_pkt, &xf_PCIE_CMD_fetch_2_PCIE_CMD_cpl, &Host_PCIE_CMD_cpl_pkt);
Host.AddTH(&th_PCIE_CMD_fetch_2_PCIE_CMD_cpl);

//----------------------------------------
//Hao: PCIE.CPR.fetch -> PCIE.CPR.cpl
//----------------------------------------
F_COND_WITH_TRANSNAME(is_pcie_cpr_fetch, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.CPR.fetch");

CSource Host_PCIE_CPR_fetch_pkt("Host-in: PCIE.CPR.fetch packet", 
				&PCIE_Egress_ch, is_pcie_cpr_fetch);
CNameTransform xf_rename_PCIE_CPR_fetch_2_PCIE_CPR_cpl ("PCIE.CPR.fetch to PCIE.CPR.cpl", "PCIE.CPR.cpl");
CSizeTransform xf_resize_PCIE_CPR_fetch_2_PCIE_CPR_cpl ("PCIE.CPR.fetch resize", 88);
     //Hao: 32bits system: 64B  + (20 ~ 24)B = 84 ~ 88B
     //     64bits system: 128B + (20 ~ 24)B = 148 ~ 152B
CGroupTransform xf_PCIE_CPR_fetch_2_PCIE_CPR_cpl("PCIE.CPR.fetch to PCIE.CPR.cpl", 1);   //Hao
xf_PCIE_CPR_fetch_2_PCIE_CPR_cpl.AddTransform(&xf_resize_PCIE_CPR_fetch_2_PCIE_CPR_cpl); //Hao
xf_PCIE_CPR_fetch_2_PCIE_CPR_cpl.AddTransform(&xf_rename_PCIE_CPR_fetch_2_PCIE_CPR_cpl); //Hao
CDestination Host_PCIE_CPR_cpl_pkt ("Host-out: PCIE.CPR.cpl packet", &PCIE_Ingress_ch,
				    2, GBps, 8);
Host_PCIE_CPR_cpl_pkt.SetPipeline(200);
CSimpleTH th_PCIE_CPR_fetch_2_PCIE_CPR_cpl ("Host: PCIE.CPR.fetch -> PCIE.CPR.cpl", 
					    &Host_PCIE_CPR_fetch_pkt, &xf_PCIE_CPR_fetch_2_PCIE_CPR_cpl, &Host_PCIE_CPR_cpl_pkt);
Host.AddTH(&th_PCIE_CPR_fetch_2_PCIE_CPR_cpl);
//Hao:end


/* ----------------------------------------
   Host: PCIE.Data.fetch -> PCIE.Data.cpl
   The size transfrom may be overwritten 
   by different applications
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_pcie_data_fetch, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.Data.fetch");
CSource Host_PCIE_Data_fetch_pkt ("Host-in: PCIE.Data.fetch packet",
				  &PCIE_Egress_ch, is_pcie_data_fetch);
//CSizeTransformPkt xf_set_cpl_size ("Set PCIE.Data.cpl size according to PCIE.Data.fetch length", new CPacketLength);
CSizeTransform      xf_set_cpl_size ("Set PCIE.Data.cpl size according to PCIE.Data.fetch length", 88);  //overhead is 5-6DW
CNameTransform xf_rename_to_PCIE_Data_cpl ("rename PCIE.Data.fetch to PCIE.Data.cpl", "PCIE.Data.cpl");
//CSplitTransform xf_split_cpl_on_size_64 ("Split PCIE.Data.cpl onSIZE 64", onSIZE, 64); remove the split
CGroupTransform xf_PCIE_Data_fetch_to_PCIE_Data_cpl ("PCIE.Data.fetch to PCIE.Data.cpl", 1);
xf_PCIE_Data_fetch_to_PCIE_Data_cpl.AddTransform(&xf_set_cpl_size);
xf_PCIE_Data_fetch_to_PCIE_Data_cpl.AddTransform(&xf_rename_to_PCIE_Data_cpl);
//xf_PCIE_Data_fetch_to_PCIE_Data_cpl.AddTransform(&xf_split_cpl_on_size_64);

CDestination Host_PCIE_Data_cpl_pkt ("Host-out: PCIE.Data.cpl packet", &PCIE_Ingress_ch,
				     2, GBps, 8);
//Hao:change the bandwidth
Host_PCIE_Data_cpl_pkt.SetPipeline(200);
//Hao:set the completion latency 100 * 8ns = 800;
CSimpleTH th_PCIE_Data_fetch_to_PCIE_Data_cpl ("Host: PCIE.Data.fetch -> PCIE.Data.cpl",
					       &Host_PCIE_Data_fetch_pkt, &xf_PCIE_Data_fetch_to_PCIE_Data_cpl, &Host_PCIE_Data_cpl_pkt);
Host.AddTH(&th_PCIE_Data_fetch_to_PCIE_Data_cpl);


// ----------------------------------------
//   Host: PCIE.WR.Data -> Host.Data
// ----------------------------------------
F_COND_WITHOUT_TRANSNAME(is_pcie_wr_data, 
			 PIPELINE, PacketName == "PCIE.WR.Data");
CSource Host_PCIE_WR_Data_pkt ("Host-in: PCIE.WR.Data packet", &PCIE_Egress_ch, is_pcie_wr_data);
CNameTransform xf_PCIE_WR_Data_to_Host_Data ("PCIE.WR.Data to Host.Data", "Host.Data");
CDestination Host_Host_Data_pkt ("Host-out: Host.Data packet", &Host_mem_ch,
				 8, GBps, 8);  //Hao: host can handle the result ASAP
//Hao:change the bandwidth
CSimpleTH th_PCIE_WR_Data_to_Host_Data ("Host: PCIE.WR.Data -> Host.Data", 
					&Host_PCIE_WR_Data_pkt, 
					&xf_PCIE_WR_Data_to_Host_Data,
					&Host_Host_Data_pkt);
Host.AddTH(&th_PCIE_WR_Data_to_Host_Data);

//----------------------------------------
//Hao: PCIE.WR.Desc -> Host.Desc
//----------------------------------------
F_COND_WITHOUT_TRANSNAME(is_pcie_wr_desc, 
			 PIPELINE, PacketName == "PCIE.WR.Desc");
CSource Host_PCIE_WR_Desc_pkt ("Host-in: PCIE.WR.Desc packet", &PCIE_Egress_ch, is_pcie_wr_desc);
CNameTransform xf_PCIE_WR_Desc_to_Host_Desc ("PCIE.WR.Desc to Host.Desc", "Host.Desc");
CDestination Host_Host_Desc_pkt ("Host-out: Host.Desc packet", &Host_mem_ch,
				 2, GBps, 8);
CSimpleTH th_PCIE_WR_Desc_to_Host_Desc ("Host: PCIE.WR.Desc -> Host.Desc", 
					&Host_PCIE_WR_Desc_pkt, 
					&xf_PCIE_WR_Desc_to_Host_Desc,
					&Host_Host_Desc_pkt);
Host.AddTH(&th_PCIE_WR_Desc_to_Host_Desc);
//Hao:end


//----------------------------------------
//Hao: add this section for CP fetch
//----------------------------------------
//Hao: DMA.CPP: CPR.Fetch.req -> CPR.fetch
F_COND_WITH_TRANSNAME(is_cpr_fetch_req, "DMA Loopback Transaction",
			 SERIAL_AHEAD, PacketName == "CPR.fetch.req");
CSource DMA_CPP_CPR_fetch_req_pkt ("DMA.CPP-in: CPR.fetch.req packet",
				   &CPR_req_ch,is_cpr_fetch_req); //use the individual channel
CNameTransform xf_CPR_fetch_req_2_CPR_fetch ("CPR.fetch.req to CPR.fetch","CPR.fetch");
CDestination DMA_CPP_CPR_fetch_pkt("DMA.CPP-out: CPR.fetch packet", &CMD_fetch_ch, 2, GBps, 8);
DMA_CPP_CPR_fetch_pkt.SetPacketDelay(800);
DMA_CPP_CPR_fetch_pkt.SetPipeline(3);

CSimpleTH th_CPR_fetch_req_2_CPR_fetch ("DMA.CPP: CPR.fetch.req -> CPR.fetch",
					&DMA_CPP_CPR_fetch_req_pkt, &xf_CPR_fetch_req_2_CPR_fetch, &DMA_CPP_CPR_fetch_pkt);
//Hao: this simple TH which will be mereged as RoundRobin TH

/* generate stimulous */
CPacketFactory CPR_fetch_req_factory ("CPR.fetch.req");
CPR_fetch_req_factory.SetLength(64); //Hao:32:64/64:128 but size transfer at the completion
CPR_fetch_req_factory.SetSize(16);  
CPR_fetch_req_factory.SetCount(100); //Hao: the number of stimulus packets
//CPR_fetch_req_factory.SetDelay(800);//Hao: add the delay for CPR request

CPMTransaction *dma_loopback_trans = TransactionFactory.NewTransaction("DMA Loopback Transaction");
DMA_CPP_CPR_fetch_req_pkt.BackDoorWrite(dma_loopback_trans, CPR_fetch_req_factory.GenPacket());




//Hao:end

/* ----------------------------------------
   DMA.CPP: CMD.Fetch.req -> CMD.fetch
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_cmd_fetch_req, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "CMD.fetch.req");

CSource DMA_CPP_CMD_fetch_req_pkt ("DMA.CPP-in: CMD.fetch.req packet", 
				   &CMD_req_ch, is_cmd_fetch_req);

CNameTransform xf_rename_CMD_fetch_req_2_CMD_fetch ("CMD.fetch.req to CMD.fetch", "CMD.fetch");
CSizeTransform xf_resize_CMD_fetch_req_2_CMD_fetch ("CMD.fetch.req to CMD.fetch", 16); //Hao: command request size
CGroupTransform xf_CMD_fetch_req_2_CMD_fetch("CMD.fetch.req to CMD.fetch", 1);    //Hao
xf_CMD_fetch_req_2_CMD_fetch.AddTransform(&xf_rename_CMD_fetch_req_2_CMD_fetch);  //Hao
xf_CMD_fetch_req_2_CMD_fetch.AddTransform(&xf_resize_CMD_fetch_req_2_CMD_fetch);  //Hao
CDestination DMA_CPP_CMD_fetch_pkt ("DMA.CPP-out: CMD.fetch packet", &CMD_fetch_ch,
				    2, GBps, 8);
DMA_CPP_CMD_fetch_pkt.SetPacketDelay(100); //Hao: command delay
//Hao:change the bandwidth
DMA_CPP_CMD_fetch_pkt.SetPipeline(3);

CSimpleTH th_CMD_fetch_req_2_CMD_fetch ("DMA.CPP: CMD.fetch.req -> CMD.fetch",
					&DMA_CPP_CMD_fetch_req_pkt, &xf_CMD_fetch_req_2_CMD_fetch, &DMA_CPP_CMD_fetch_pkt);

//Hao: New added


/* /\* generate stimulous *\/ */
/* CPacketFactory CMD_fetch_req_factory ("CMD.fetch.req"); */
/* CMD_fetch_req_factory.SetLength(64); */
/* CMD_fetch_req_factory.SetSize(16); */
/* //New: change size */
/* CMD_fetch_req_factory.SetCount(2); */
/* //Hao:change */
/* CMD_fetch_req_factory.SetDelay(0); */

/* CPMTransaction *dma_loopback_trans = TransactionFactory.NewTransaction("DMA Loopback Transaction"); */
/* DMA_CPP_CMD_fetch_req_pkt.BackDoorWrite(dma_loopback_trans, CMD_fetch_req_factory.GenPacket()); */

//----------------------------------------
//Hao: combine the CPR.fetch and CMD.fetch
//----------------------------------------
CRoundRobinTH th_DMA_CPP_rdreq_rr ("DMA.CPP read request RR");
th_DMA_CPP_rdreq_rr.AddTH(&th_CMD_fetch_req_2_CMD_fetch);//Add the weigth?
th_DMA_CPP_rdreq_rr.AddTH(&th_CPR_fetch_req_2_CPR_fetch);//Add the weight?
DMA_CPP.AddTH(&th_DMA_CPP_rdreq_rr); //Add the RR TH to CPP
//DMA_CPP.AddTH(&th_CPR_fetch_req_2_CPR_fetch);
//Hao:end


/* ----------------------------------------
   DMA.CPP: CMD.cpl -> EU.cmd
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_cmd_cpl, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "CMD.cpl"); //Hao: pop completion with serial_ahead mode
CSource DMA_CPP_CMD_cpl_pkt ("DMA.CPP-in: CMD.cpl packet", &CMD_cpl_ch, is_cmd_cpl);
CNameTransform xf_CMD_cpl_to_EU_cmd ("CMD.cpl to EU.cmd", "EU.cmd");
CDestination DMA_CPP_EU_cmd_pkt ("DMA.CPP-out: EU.cmd packet", &CMD_fwd_ch,
				 2, GBps, 8);
//Hao:change the bandwidth
DMA_CPP_EU_cmd_pkt.SetPipeline(3);

CSimpleTH th_CMD_cpl_to_EU_cmd ("DMA.CPP CMD.cpl -> EU.cmd",
				&DMA_CPP_CMD_cpl_pkt, &xf_CMD_cpl_to_EU_cmd, &DMA_CPP_EU_cmd_pkt);

//DMA_CPP.AddTH(&th_CMD_cpl_to_EU_cmd);
//Hao: replace with dispath TH

//----------------------------------------
//Hao: DMA.CPP  CPR.cpl ->  CPP.cmd
//----------------------------------------
F_COND_WITH_TRANSNAME(is_cpr_cpl, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "CPR.cpl"); //Hao: pop completion with serial_ahead mode
CSource DMA_CPP_CPR_cpl_pkt ("DMA.CPP-in: CPR.cpl packet", &CMD_cpl_ch, is_cpr_cpl);
CNameTransform xf_CPR_cpl_to_CPP_cmd ("CPR.cpl to CPP.cmd", "CPP.cmd");
CDestination DMA_CPP_CPP_cmd_pkt ("DMA.CPP-out: CPP.cmd packet", &CPR_fwd_ch,
				 2, GBps, 8);
DMA_CPP_CPP_cmd_pkt.SetPipeline(3);

CSimpleTH th_CPR_cpl_to_CPP_cmd ("DMA.CPP CPR.cpl -> CPP.cmd",
				&DMA_CPP_CPR_cpl_pkt, &xf_CPR_cpl_to_CPP_cmd, &DMA_CPP_CPP_cmd_pkt);

CDispatchTH th_DMA_CPP_cpl_dispatch("DMA.CPP: Dispatch cpl to CPR channel or CMD channel");
th_DMA_CPP_cpl_dispatch.AddTH(&th_CMD_cpl_to_EU_cmd);
th_DMA_CPP_cpl_dispatch.AddTH(&th_CPR_cpl_to_CPP_cmd);
DMA_CPP.AddTH(&th_DMA_CPP_cpl_dispatch);//add dispatch path!
//Hao:end

/* ----------------------------------------
   IDCx common conditions and XFMs
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_data_fetch_req, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "Data.fetch.req");

CNameTransform  xf_rename_Data_fetch_req_2_Data_fetch ("Rename Data.fetch.req to Data.fetch","Data.fetch");
CSplitTransform xf_split_data_fetch_64 ("Split Data.fetch.req, chunk size is 64", onLENGTH, 64);
CSplitTransform xf_split_data_fetch_128 ("Split Data.fetch.req, chunk size is 128", onLENGTH, 128);
CSplitTransform xf_split_data_fetch_256 ("Split Data.fetch.req, chunk size is 256", onLENGTH, 256);
CRandomTransform xf_random_split("Random split Data.fetch.req", 1);
xf_random_split.AddTransform(&xf_split_data_fetch_64);
xf_random_split.AddTransform(&xf_split_data_fetch_128);
xf_random_split.AddTransform(&xf_split_data_fetch_256);

CGroupTransform xf_common_Data_fetch_req_2_Data_fetch ("Common Data.fetch.req to Data.fetch", 1);
xf_common_Data_fetch_req_2_Data_fetch.AddTransform(&xf_rename_Data_fetch_req_2_Data_fetch);
xf_common_Data_fetch_req_2_Data_fetch.AddTransform(&xf_random_split);


//----------------------------------------
//Hao: Data.cpl -> EU.data
//----------------------------------------
CNameTransform xf_common_Data_cpl_2_EU_data ("Common Data.cpl to EU.data", "EU.data");


/* ----------------------------------------
   IDC1: Data.fetch.req -> Data.fetch
   ----------------------------------------*/
CSource DMA_IDC1_Data_fetch_req_pkt ("DMA.IDC1-in: Data.fetch.req packet", &IDC1_req_ch, is_data_fetch_req);

CCustomTransform xf_IDC1_add_req_tag ("Tag Data.fetch with IDC1", "requester", "IDC1");
CGroupTransform xf_IDC1_Data_fetch_req_to_Data_fetch ("Data.fetch.req to Data.fetch (IDC1)", 1);
xf_IDC1_Data_fetch_req_to_Data_fetch.AddTransform(&xf_common_Data_fetch_req_2_Data_fetch);
xf_IDC1_Data_fetch_req_to_Data_fetch.AddTransform(&xf_IDC1_add_req_tag);

CDestination DMA_IDC1_data_fetch_pkt("DMA.IDC1-out: Data.fetch packet", &IDC1_out_ch,
				     2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC1_data_fetch_pkt.SetPipeline(3);

CSimpleTH th_IDC1_Data_fetch_req_2_Data_fetch ("DMA.IDC1: Data.fetch.req -> Data.fetch", 
					       &DMA_IDC1_Data_fetch_req_pkt, 
					       &xf_IDC1_Data_fetch_req_to_Data_fetch, 
					       &DMA_IDC1_data_fetch_pkt);
/* ----------------------------------------
   IDC1: Data.cpl -> EU.data
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_IDC1_data_cpl, "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Data.cpl" && CustomFields["requester"] == "IDC1");

CSource DMA_IDC1_Data_cpl_pkt ("DMA.IDC1-in: Data.cpl packet", 
			       &IDC1_in_ch, is_IDC1_data_cpl);
CDestination DMA_IDC1_EU_data_pkt ("DMA.IDC1-out: EU.data packet", &IDC1_cpl_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC1_EU_data_pkt.SetPipeline(3);

CSimpleTH th_IDC1_Data_cpl_2_EU_data ("DMA.IDC1: Data.cpl -> EU.data",
				      &DMA_IDC1_Data_cpl_pkt, 
				      &xf_common_Data_cpl_2_EU_data, 
				      &DMA_IDC1_EU_data_pkt);

DMA_IDC1.AddTH(&th_IDC1_Data_fetch_req_2_Data_fetch);
DMA_IDC1.AddTH(&th_IDC1_Data_cpl_2_EU_data);






/* ----------------------------------------
   IDC2: Data.fetch.req -> Data.fetch
   ----------------------------------------*/
CSource DMA_IDC2_Data_fetch_req_pkt ("DMA.IDC2-in: Data.fetch.req packet", &IDC2_req_ch, is_data_fetch_req);

CCustomTransform xf_IDC2_add_req_tag ("Tag Data.fetch with IDC2", "requester", "IDC2");
CGroupTransform xf_IDC2_Data_fetch_req_to_Data_fetch ("Data.fetch.req to Data.fetch (IDC2)", 1);
xf_IDC2_Data_fetch_req_to_Data_fetch.AddTransform(&xf_common_Data_fetch_req_2_Data_fetch);
xf_IDC2_Data_fetch_req_to_Data_fetch.AddTransform(&xf_IDC2_add_req_tag);

CDestination DMA_IDC2_data_fetch_pkt("DMA.IDC2-out: Data.fetch packet", &IDC2_out_ch,
				     2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC2_data_fetch_pkt.SetPipeline(3);

CSimpleTH th_IDC2_Data_fetch_req_2_Data_fetch ("DMA.IDC2: Data.fetch.req -> Data.fetch", 
					       &DMA_IDC2_Data_fetch_req_pkt, 
					       &xf_IDC2_Data_fetch_req_to_Data_fetch, 
					       &DMA_IDC2_data_fetch_pkt);
/* ----------------------------------------
   IDC2: Data.cpl -> EU.data
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_IDC2_data_cpl, "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Data.cpl" && CustomFields["requester"] == "IDC2");

CSource DMA_IDC2_Data_cpl_pkt ("DMA.IDC2-in: Data.cpl packet", 
			       &IDC2_in_ch, is_IDC2_data_cpl);
CDestination DMA_IDC2_EU_data_pkt ("DMA.IDC2-out: EU.data packet", &IDC2_cpl_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC2_EU_data_pkt.SetPipeline(3);

CSimpleTH th_IDC2_Data_cpl_2_EU_data ("DMA.IDC2: Data.cpl -> EU.data",
				      &DMA_IDC2_Data_cpl_pkt, 
				      &xf_common_Data_cpl_2_EU_data, 
				      &DMA_IDC2_EU_data_pkt);

DMA_IDC2.AddTH(&th_IDC2_Data_fetch_req_2_Data_fetch);
DMA_IDC2.AddTH(&th_IDC2_Data_cpl_2_EU_data);






/* ----------------------------------------
   IDC3: Data.fetch.req -> Data.fetch
   ----------------------------------------*/
CSource DMA_IDC3_Data_fetch_req_pkt ("DMA.IDC3-in: Data.fetch.req packet", &IDC3_req_ch, is_data_fetch_req);

CCustomTransform xf_IDC3_add_req_tag ("Tag Data.fetch with IDC3", "requester", "IDC3");
CGroupTransform xf_IDC3_Data_fetch_req_to_Data_fetch ("Data.fetch.req to Data.fetch (IDC3)", 1);
xf_IDC3_Data_fetch_req_to_Data_fetch.AddTransform(&xf_common_Data_fetch_req_2_Data_fetch);
xf_IDC3_Data_fetch_req_to_Data_fetch.AddTransform(&xf_IDC3_add_req_tag);

CDestination DMA_IDC3_data_fetch_pkt("DMA.IDC3-out: Data.fetch packet", &IDC3_out_ch,
				     2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC3_data_fetch_pkt.SetPipeline(3);

CSimpleTH th_IDC3_Data_fetch_req_2_Data_fetch ("DMA.IDC3: Data.fetch.req -> Data.fetch", 
					       &DMA_IDC3_Data_fetch_req_pkt, 
					       &xf_IDC3_Data_fetch_req_to_Data_fetch, 
					       &DMA_IDC3_data_fetch_pkt);
/* ----------------------------------------
   IDC3: Data.cpl -> EU.data
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_IDC3_data_cpl, "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Data.cpl" && CustomFields["requester"] == "IDC3");

CSource DMA_IDC3_Data_cpl_pkt ("DMA.IDC3-in: Data.cpl packet", 
			       &IDC3_in_ch, is_IDC3_data_cpl);
CDestination DMA_IDC3_EU_data_pkt ("DMA.IDC3-out: EU.data packet", &IDC3_cpl_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC3_EU_data_pkt.SetPipeline(3);

CSimpleTH th_IDC3_Data_cpl_2_EU_data ("DMA.IDC3: Data.cpl-> EU.data",
				      &DMA_IDC3_Data_cpl_pkt, 
				      &xf_common_Data_cpl_2_EU_data, 
				      &DMA_IDC3_EU_data_pkt);

DMA_IDC3.AddTH(&th_IDC3_Data_fetch_req_2_Data_fetch);
DMA_IDC3.AddTH(&th_IDC3_Data_cpl_2_EU_data);





/* ----------------------------------------
   IDC4: Data.fetch.req -> Data.fetch
   ----------------------------------------*/
CSource DMA_IDC4_Data_fetch_req_pkt ("DMA.IDC4-in: Data.fetch.req packet", &IDC4_req_ch, is_data_fetch_req);

CCustomTransform xf_IDC4_add_req_tag ("Tag Data.fetch with IDC4", "requester", "IDC4");
CGroupTransform xf_IDC4_Data_fetch_req_to_Data_fetch ("Data.fetch.req to Data.fetch (IDC4)", 1);
xf_IDC4_Data_fetch_req_to_Data_fetch.AddTransform(&xf_common_Data_fetch_req_2_Data_fetch);
xf_IDC4_Data_fetch_req_to_Data_fetch.AddTransform(&xf_IDC4_add_req_tag);

CDestination DMA_IDC4_data_fetch_pkt("DMA.IDC4-out: Data.fetch packet", &IDC4_out_ch,
				     2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC4_data_fetch_pkt.SetPipeline(3);

CSimpleTH th_IDC4_Data_fetch_req_2_Data_fetch ("DMA.IDC4: Data.fetch.req -> Data.fetch", 
					       &DMA_IDC4_Data_fetch_req_pkt, 
					       &xf_IDC4_Data_fetch_req_to_Data_fetch, 
					       &DMA_IDC4_data_fetch_pkt);
/* ----------------------------------------
   IDC4: Data.cpl -> EU.data
   ----------------------------------------*/
F_COND_WITH_TRANSNAME(is_IDC4_data_cpl, "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Data.cpl" && CustomFields["requester"] == "IDC4");

CSource DMA_IDC4_Data_cpl_pkt ("DMA.IDC4-in: Data.cpl packet", 
			       &IDC4_in_ch, is_IDC4_data_cpl);
CDestination DMA_IDC4_EU_data_pkt ("DMA.IDC4-out: EU.data packet", &IDC4_cpl_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
DMA_IDC4_EU_data_pkt.SetPipeline(3);

CSimpleTH th_IDC4_Data_cpl_2_EU_data ("DMA.IDC4: Data.cpl -> EU.data",
				      &DMA_IDC4_Data_cpl_pkt, 
				      &xf_common_Data_cpl_2_EU_data, 
				      &DMA_IDC4_EU_data_pkt);

DMA_IDC4.AddTH(&th_IDC4_Data_fetch_req_2_Data_fetch);
DMA_IDC4.AddTH(&th_IDC4_Data_cpl_2_EU_data);








/* ----------------------------------------
   DMA.RRC Data.fetch -> PCIE.Data.fetch
   ----------------------------------------*/
F_COND_WITH_TRANSNAME (is_data_fetch,  "DMA Loopback Transaction", 
		       SERIAL_AHEAD, PacketName == "Data.fetch");
CNameTransform xf_Data_fetch_to_PCIE_Data_fetch ("Common Data.fetch to PCIE.Data.fetch", "PCIE.Data.fetch");


CSource DMA_RRC_Data_fetch_pkt_IDC1 ("DMA.RRC-in: Data.fetch packet on IDC1", &IDC1_out_ch, is_data_fetch);
CDestination DMA_RRC_PCIE_Data_fetch_pkt_IDC1 ("DMA.RRC-out: PCIE.Data.fetch on IDC1", &PCIE_Egress_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
DMA_RRC_PCIE_Data_fetch_pkt_IDC1.SetPipeline(18);
//Hao: add the delay from RRC->PCIE
CSimpleTH th_Data_fetch_to_PCIE_Data_fetch_IDC1 ("DMA.RRC: Data.fetch -> PCIE.Data.fetch (IDC1)", 
						 &DMA_RRC_Data_fetch_pkt_IDC1,
						 &xf_Data_fetch_to_PCIE_Data_fetch,
						 &DMA_RRC_PCIE_Data_fetch_pkt_IDC1);



CSource DMA_RRC_Data_fetch_pkt_IDC2 ("DMA.RRC-in: Data.fetch packet on IDC2", &IDC2_out_ch, is_data_fetch);
CDestination DMA_RRC_PCIE_Data_fetch_pkt_IDC2 ("DMA.RRC-out: PCIE.Data.fetch on IDC2", &PCIE_Egress_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
DMA_RRC_PCIE_Data_fetch_pkt_IDC2.SetPipeline(18);
//Hao: add the delay from RRC->PCIE
CSimpleTH th_Data_fetch_to_PCIE_Data_fetch_IDC2 ("DMA.RRC: Data.fetch -> PCIE.Data.fetch (IDC2)", 
						 &DMA_RRC_Data_fetch_pkt_IDC2,
						 &xf_Data_fetch_to_PCIE_Data_fetch,
						 &DMA_RRC_PCIE_Data_fetch_pkt_IDC2);



CSource DMA_RRC_Data_fetch_pkt_IDC3 ("DMA.RRC-in: Data.fetch packet on IDC3", &IDC3_out_ch, is_data_fetch);
CDestination DMA_RRC_PCIE_Data_fetch_pkt_IDC3 ("DMA.RRC-out: PCIE.Data.fetch on IDC3", &PCIE_Egress_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
DMA_RRC_PCIE_Data_fetch_pkt_IDC3.SetPipeline(18);
//Hao: add the delay from RRC->PCIE
CSimpleTH th_Data_fetch_to_PCIE_Data_fetch_IDC3 ("DMA.RRC: Data.fetch -> PCIE.Data.fetch (IDC3)", 
						 &DMA_RRC_Data_fetch_pkt_IDC3,
						 &xf_Data_fetch_to_PCIE_Data_fetch,
						 &DMA_RRC_PCIE_Data_fetch_pkt_IDC3);




CSource DMA_RRC_Data_fetch_pkt_IDC4 ("DMA.RRC-in: Data.fetch packet on IDC4", &IDC4_out_ch, is_data_fetch);
CDestination DMA_RRC_PCIE_Data_fetch_pkt_IDC4 ("DMA.RRC-out: PCIE.Data.fetch on IDC4", &PCIE_Egress_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
DMA_RRC_PCIE_Data_fetch_pkt_IDC4.SetPipeline(18);
//Hao: add the delay from RRC->PCIE
CSimpleTH th_Data_fetch_to_PCIE_Data_fetch_IDC4 ("DMA.RRC: Data.fetch -> PCIE.Data.fetch (IDC4)", 
						 &DMA_RRC_Data_fetch_pkt_IDC4,
						 &xf_Data_fetch_to_PCIE_Data_fetch,
						 &DMA_RRC_PCIE_Data_fetch_pkt_IDC4);




F_COND_WITH_TRANSNAME(is_cmd_fetch,  "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "CMD.fetch");
CSource DMA_RRC_CMD_fetch_pkt ("DMA.RRC-in: CMD.fetch packet", 
			       &CMD_fetch_ch, is_cmd_fetch);
CNameTransform xf_CMD_fetch_to_PCIE_CMD_fetch ("CMD.fetch to PCIE.CMD.fetch", "PCIE.CMD.fetch");
CDestination DMA_RRC_PCIE_CMD_fetch_pkt ("DMA.RRC-out: PCIE.CMD.fetch", &PCIE_Egress_ch,
					 2, GBps, 8);
//Hao:change the bandwidth
DMA_RRC_PCIE_CMD_fetch_pkt.SetPipeline(18);
//Hao:add the transmit latency
CSimpleTH th_CMD_fetch_to_PCIE_CMD_fetch ("DMA.RRC: CMD.fetch -> PCIE.CMD.fetch",
					  &DMA_RRC_CMD_fetch_pkt, 
					  &xf_CMD_fetch_to_PCIE_CMD_fetch,
					  &DMA_RRC_PCIE_CMD_fetch_pkt);

//----------------------------------------
//Hao:add the CPR read request TH
//----------------------------------------
F_COND_WITH_TRANSNAME(is_cpr_fetch, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "CPR.fetch");
CSource DMA_RRC_CPR_fetch_pkt ("DMA.RRC-in: CPR.fetch packet",
			       &CMD_fetch_ch, is_cpr_fetch);
CNameTransform xf_CPR_fetch_to_PCIE_CMD_fetch ("CPR.fetch to PCIE.CPR.fetch", "PCIE.CPR.fetch");
CDestination DMA_RRC_PCIE_CPR_fetch_pkt ("DMA.RRC-out: PCIE.CPR.fetch", &PCIE_Egress_ch,
					 2, GBps, 8);
DMA_RRC_PCIE_CPR_fetch_pkt.SetPipeline(18);
CSimpleTH th_CPR_fetch_to_PCIE_CPR_fetch ("DMA.RRC: CPR.fetch -> PCIE.CPR.fetch",
					  &DMA_RRC_CPR_fetch_pkt,
					  &xf_CPR_fetch_to_PCIE_CMD_fetch,
					  &DMA_RRC_PCIE_CPR_fetch_pkt);
//Hao:end

CRoundRobinTH th_DMA_RRC_rdreq_dispatch ("DMA.RRC read request Dispatch");
th_DMA_RRC_rdreq_dispatch.AddTH(&th_Data_fetch_to_PCIE_Data_fetch_IDC1,1);
th_DMA_RRC_rdreq_dispatch.AddTH(&th_Data_fetch_to_PCIE_Data_fetch_IDC2,1);
th_DMA_RRC_rdreq_dispatch.AddTH(&th_Data_fetch_to_PCIE_Data_fetch_IDC3,1);
th_DMA_RRC_rdreq_dispatch.AddTH(&th_Data_fetch_to_PCIE_Data_fetch_IDC4,1);
//th_DMA_RRC_rdreq_dispatch.AddTH(&th_DMA_CPP_rdreq_rr,4);  //Hao: use the RR TH here,weight?
th_DMA_RRC_rdreq_dispatch.AddTH(&th_CPR_fetch_to_PCIE_CPR_fetch,4);  //Hao: use the RR TH here,weight?
th_DMA_RRC_rdreq_dispatch.AddTH(&th_CMD_fetch_to_PCIE_CMD_fetch,4);  //Hao: use the RR TH here,weight?
//Hao:add the weight among the five sources!
//four commands will initiate 4 packets read source DATA requests

DMA_RRC.AddTH(&th_DMA_RRC_rdreq_dispatch);



// ----------------------------------------
//  DMA.CC: PCIE.CMD.cpl -> CMD.cpl
// ----------------------------------------
F_COND_WITH_TRANSNAME(is_pcie_cmd_cpl, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.CMD.cpl");
CSource DMA_CC_PCIE_CMD_cpl_pkt ("DMA.CC-in: PCIE.CMD.cpl packet", &PCIE_Ingress_ch, is_pcie_cmd_cpl);
CNameTransform xf_rename_PCIE_CMD_cpl_to_CMD_cpl ("PCIE.CMD.cpl to CMD.cpl", "CMD.cpl");
CSizeTransform xf_resize_PCIE_CMD_cpl_to_CMD_cpl ("Set CMD.cpl size", 32); //Hao:32bits system
//CSizeTransform xf_resize_PCIE_CMD_cpl_to_CMD_cpl ("Set CMD.cpl size", 48); //Hao:64bits system
     //Hao:resize the command completion
     //32bits system:16B + 16B + 16B * (0-3)  = 32B + 16B * (0-3)
     //64bits system:16B + 32B + 32B * (0-2)  = 48B + 32B * (0-2)
CGroupTransform xf_PCIE_CMD_cpl_to_CMD_cpl("PCIE.CMD.cpl to CMD.cpl", 1);
xf_PCIE_CMD_cpl_to_CMD_cpl.AddTransform(&xf_rename_PCIE_CMD_cpl_to_CMD_cpl);
xf_PCIE_CMD_cpl_to_CMD_cpl.AddTransform(&xf_resize_PCIE_CMD_cpl_to_CMD_cpl);


CDestination DMA_CC_CMD_cpl_pkt ("DMA.CC-out: CMD.cpl packet", &CMD_cpl_ch,
				 2, GBps, 8);
//Hao:change the bandwidth
DMA_CC_CMD_cpl_pkt.SetPipeline(35);
//Hao:change the delay cycles from 3 -> 35 PCIE PAD -> PCIE core 18cycles
//                                         PCIE core -> CC       17cycles

CSimpleTH th_PCIE_CMD_cpl_to_CMD_cpl ("DMA.CC: PCIE.CMD.cpl -> CMD.cpl",
				      &DMA_CC_PCIE_CMD_cpl_pkt,
				      &xf_PCIE_CMD_cpl_to_CMD_cpl,
				      &DMA_CC_CMD_cpl_pkt);

//----------------------------------------
//Hao: DMA.CC: PCIE.CPR.cpl -> CPR.cpl
//----------------------------------------
F_COND_WITH_TRANSNAME(is_pcie_cpr_cpl, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.CPR.cpl");
CSource DMA_CC_PCIE_CPR_cpl_pkt ("DMA.CC-in: PCIE.CPR.cpl packet", &PCIE_Ingress_ch, is_pcie_cpr_cpl);
CNameTransform xf_rename_PCIE_CPR_cpl_to_CPR_cpl ("PCIE.CPR.cpl to CPR.cpl", "CPR.cpl");
CSizeTransform xf_resize_PCIE_CPR_cpl_to_CPR_cpl ("PCIE.CPR.cpl to CPR.cpl", 64); //Hao:32bits system
//CSizeTransform xf_resize_PCIE_CPR_cpl_to_CPR_cpl ("PCIE.CPR.cpl to CPR.cpl", 128); //Hao:64bits system
CGroupTransform xf_PCIE_CPR_cpl_to_CPR_cpl("PCIE.CPR.cpl to CPR.cpl", 1);    //Hao
xf_PCIE_CPR_cpl_to_CPR_cpl.AddTransform(&xf_rename_PCIE_CPR_cpl_to_CPR_cpl); //Hao
xf_PCIE_CPR_cpl_to_CPR_cpl.AddTransform(&xf_resize_PCIE_CPR_cpl_to_CPR_cpl); //Hao
CDestination DMA_CC_CPR_cpl_pkt ("DMA.CC-out: CPR.cpl packet", &CMD_cpl_ch,
				 2, GBps, 8);  //Hao:reuse the CMD_cpl_ch!
DMA_CC_CPR_cpl_pkt.SetPipeline(35);
//Hao:change the delay cycles from 3 -> 35 PCIE PAD -> PCIE core 18cycles
//                                         PCIE core -> CC       17cycles
CSimpleTH th_PCIE_CPR_cpl_to_CPR_cpl ("DMA.CC: PCIE.CPR.cpl -> CPR.cpl",
				      &DMA_CC_PCIE_CPR_cpl_pkt,
				      &xf_PCIE_CPR_cpl_to_CPR_cpl,
				      &DMA_CC_CPR_cpl_pkt);
//Hao:end
//----------------------------------------
//Hao: DMA.CC: PCIE.Data.cpl -> Data.cpl
//----------------------------------------
CNameTransform xf_rename_PCIE_Data_cpl_to_Data_cpl ("Common PCIE.Data.cpl to Data.cpl", "Data.cpl");
CSizeTransform xf_size_PCIE_Data_cpl_to_Data_cpl("remove the PCIE overhead in this stage", 64); //Hao:use the direct value
CGroupTransform xf_PCIE_Data_cpl_to_Data_cpl ("PCIE_Data_cpl tp Data.cpl", 1);      //Hao: use the group transfer here
xf_PCIE_Data_cpl_to_Data_cpl.AddTransform(&xf_rename_PCIE_Data_cpl_to_Data_cpl);    //Hao: include rename transfer
xf_PCIE_Data_cpl_to_Data_cpl.AddTransform(&xf_size_PCIE_Data_cpl_to_Data_cpl);      //Hao: include size transfer
F_COND_WITH_TRANSNAME(is_pcie_data_cpl_IDC1, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.Data.cpl" && CustomFields["requester"]=="IDC1");
CSource DMA_CC_PCIE_Data_cpl_pkt_IDC1 ("DMA.CC-in: PCIE.Data.cpl packet on IDC1", &PCIE_Ingress_ch, is_pcie_data_cpl_IDC1);
CDestination DMA_CC_Data_cpl_pkt_IDC1 ("DMA.CC-out: Data.cpl packet on IDC1", &IDC1_in_ch,
				       2, GBps, 8);
//Hao:change the bandwidth
DMA_CC_Data_cpl_pkt_IDC1.SetPipeline(35);
//Hao:change the delay cycles from 3 -> 35 PCIE PAD -> PCIE core 18cycles
//                                         PCIE core -> CC       17cycles

CSimpleTH th_PCIE_Data_cpl_to_Data_cpl_IDC1 ("DMA.CC: PCIE.Data.cpl -> Data.cpl (IDC1)",
					     &DMA_CC_PCIE_Data_cpl_pkt_IDC1,
					     &xf_PCIE_Data_cpl_to_Data_cpl,
					     &DMA_CC_Data_cpl_pkt_IDC1);



F_COND_WITH_TRANSNAME(is_pcie_data_cpl_IDC2, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.Data.cpl" && CustomFields["requester"]=="IDC2");
CSource DMA_CC_PCIE_Data_cpl_pkt_IDC2 ("DMA.CC-in: PCIE.Data.cpl packet on IDC2", &PCIE_Ingress_ch, is_pcie_data_cpl_IDC2);
CDestination DMA_CC_Data_cpl_pkt_IDC2 ("DMA.CC-out: Data.cpl packet on IDC2", &IDC2_in_ch,
				       2, GBps, 8);
//Hao:change the bandwidth
DMA_CC_Data_cpl_pkt_IDC2.SetPipeline(35);
//Hao:change the delay cycles from 3 -> 35 PCIE PAD -> PCIE core 18cycles
//                                         PCIE core -> CC       17cycles

CSimpleTH th_PCIE_Data_cpl_to_Data_cpl_IDC2 ("DMA.CC: PCIE.Data.cpl -> Data.cpl (IDC2)",
					     &DMA_CC_PCIE_Data_cpl_pkt_IDC2,
					     &xf_PCIE_Data_cpl_to_Data_cpl,
					     &DMA_CC_Data_cpl_pkt_IDC2);



F_COND_WITH_TRANSNAME(is_pcie_data_cpl_IDC3, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.Data.cpl" && CustomFields["requester"]=="IDC3");
CSource DMA_CC_PCIE_Data_cpl_pkt_IDC3 ("DMA.CC-in: PCIE.Data.cpl packet on IDC3", &PCIE_Ingress_ch, is_pcie_data_cpl_IDC3);
CDestination DMA_CC_Data_cpl_pkt_IDC3 ("DMA.CC-out: Data.cpl packet on IDC3", &IDC3_in_ch,
				       2, GBps, 8);
//Hao:change the bandwidth
DMA_CC_Data_cpl_pkt_IDC3.SetPipeline(35);
//Hao:change the delay cycles from 3 -> 35 PCIE PAD -> PCIE core 18cycles
//                                         PCIE core -> CC       17cycles

CSimpleTH th_PCIE_Data_cpl_to_Data_cpl_IDC3 ("DMA.CC: PCIE.Data.cpl -> Data.cpl (IDC3)",
					     &DMA_CC_PCIE_Data_cpl_pkt_IDC3,
					     &xf_PCIE_Data_cpl_to_Data_cpl,
					     &DMA_CC_Data_cpl_pkt_IDC3);



F_COND_WITH_TRANSNAME(is_pcie_data_cpl_IDC4, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "PCIE.Data.cpl" && CustomFields["requester"]=="IDC4");
CSource DMA_CC_PCIE_Data_cpl_pkt_IDC4 ("DMA.CC-in: PCIE.Data.cpl packet on IDC4", &PCIE_Ingress_ch, is_pcie_data_cpl_IDC4);
CDestination DMA_CC_Data_cpl_pkt_IDC4 ("DMA.CC-out: Data.cpl packet on IDC4", &IDC4_in_ch,
				       2, GBps, 8);
//Hao:change the bandwidth
DMA_CC_Data_cpl_pkt_IDC4.SetPipeline(35);
//Hao:change the delay cycles from 3 -> 35 PCIE PAD -> PCIE core 18cycles
//                                         PCIE core -> CC       17cycles
CSimpleTH th_PCIE_Data_cpl_to_Data_cpl_IDC4 ("DMA.CC: PCIE.Data.cpl -> Data.cpl (IDC4)",
					     &DMA_CC_PCIE_Data_cpl_pkt_IDC4,
					     &xf_PCIE_Data_cpl_to_Data_cpl,
					     &DMA_CC_Data_cpl_pkt_IDC4);




CDispatchTH th_DMA_CC_cpl_dispatch("DMA.CC: Dispatch cpl to IDCx or CPP");
th_DMA_CC_cpl_dispatch.AddTH(&th_PCIE_CPR_cpl_to_CPR_cpl,4); //Hao: weight of CPR completion?
th_DMA_CC_cpl_dispatch.AddTH(&th_PCIE_CMD_cpl_to_CMD_cpl,4);
th_DMA_CC_cpl_dispatch.AddTH(&th_PCIE_Data_cpl_to_Data_cpl_IDC1,1);
th_DMA_CC_cpl_dispatch.AddTH(&th_PCIE_Data_cpl_to_Data_cpl_IDC2,1);
th_DMA_CC_cpl_dispatch.AddTH(&th_PCIE_Data_cpl_to_Data_cpl_IDC3,1);
th_DMA_CC_cpl_dispatch.AddTH(&th_PCIE_Data_cpl_to_Data_cpl_IDC4,1);
//Hao: 4 commands will fetch 4 source packets

DMA_CC.AddTH(&th_DMA_CC_cpl_dispatch);


//----------------------------------------
//Hao: CMD_TRANS
//CPP.cmd -> CMD.fetch.req
//----------------------------------------
F_COND_WITH_TRANSNAME(is_cpp_cmd, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "CPP.cmd");
CNameTransform xf_rename_to_CMD_fetch_req ("rename CPP.cmd to CMD.fetch.req", "CMD.fetch.req");
//CLengthTransform xf_random_length_CMD_fetch ("set random length in CMD.fetch.req", 128, 250, 2);
CSplitTransform xf_split_cpr_4 ("Split CPR.cpl onSIZE 4", onSIZE, 2); //Hao: 32bits system
//CSplitTransform xf_split_cpr_8 ("Split CPR.cpl onSIZE 8", onSIZE, 8); //Hao: 64bits system
CGroupTransform xf_CPP_cmd_to_CMD_fetch_req ("CPP.cmd to CMD.fetch.req", 1);
xf_CPP_cmd_to_CMD_fetch_req.AddTransform(&xf_rename_to_CMD_fetch_req);
xf_CPP_cmd_to_CMD_fetch_req.AddTransform(&xf_split_cpr_4);

CSource CMD_TRANS_CPP_cmd_pkt ("CMD_TRANS-in: CPP.cmd packet", 
			       &CPR_fwd_ch, is_cpp_cmd);
CDestination CMD_TRANS_CMD_fetch_req_pkt ("CMD_TRANS-out: CMD.fetch.req packet", &CMD_req_ch,
					  2, GBps, 8);
//CMD_TRANS_CMD_fetch_req_pkt.SetPacketDelay(20); //Hao:
CSimpleTH th_CPP_cmd_to_CMD_fetch_req ("CMD_TRANS: CPP.cmd -> CMD.fetch.req",
				       &CMD_TRANS_CPP_cmd_pkt,
				       &xf_CPP_cmd_to_CMD_fetch_req,
				       &CMD_TRANS_CMD_fetch_req_pkt);
CMD_TRANS.AddTH(&th_CPP_cmd_to_CMD_fetch_req);
//Hao:end




// ----------------------------------------
//   XBAR_CPL: EU.data -> Mem.Data
// ----------------------------------------
F_COND_WITH_TRANSNAME (is_data_cpl, "DMA Loopback Transaction", 
		       PIPELINE, PacketName == "EU.data");
CNameTransform xf_EU_data_to_Mem_Data ("EU.data to Mem.Data", "Mem.Data");


CSource XBAR_CPL_EU_data_pkt_IDC1 ("XBAR_CPL-in: EU.data packet (IDC1)", &IDC1_cpl_ch, is_data_cpl);
CDestination XBAR_CPL_Mem_Data_pkt_IDC1 ("XBAR_CPL-out: Mem.Data (IDC1)", &Mem_Data_in_ch,
					 2, GBps, 8);
//Hao:change the bandwidth
XBAR_CPL_Mem_Data_pkt_IDC1.SetPipeline(3);
CSimpleTH th_EU_data_to_Mem_Data_IDC1 ("XBAR_CPL: EU.data -> Mem.Data (IDC1)",
				       &XBAR_CPL_EU_data_pkt_IDC1, 
				       &xf_EU_data_to_Mem_Data, 
				       &XBAR_CPL_Mem_Data_pkt_IDC1);



CSource XBAR_CPL_EU_data_pkt_IDC2 ("XBAR_CPL-in: EU.data packet (IDC2)", &IDC2_cpl_ch, is_data_cpl);
CDestination XBAR_CPL_Mem_Data_pkt_IDC2 ("XBAR_CPL-out: Mem.Data (IDC2)", &Mem_Data_in_ch,
					 2, GBps, 8);
//Hao:change the bandwidth
XBAR_CPL_Mem_Data_pkt_IDC2.SetPipeline(3);
CSimpleTH th_EU_data_to_Mem_Data_IDC2 ("XBAR_CPL: EU.data -> Mem.Data (IDC2)",
				       &XBAR_CPL_EU_data_pkt_IDC2, 
				       &xf_EU_data_to_Mem_Data, 
				       &XBAR_CPL_Mem_Data_pkt_IDC2);



CSource XBAR_CPL_EU_data_pkt_IDC3 ("XBAR_CPL-in: EU.data packet (IDC3)", &IDC3_cpl_ch, is_data_cpl);
CDestination XBAR_CPL_Mem_Data_pkt_IDC3 ("XBAR_CPL-out: Mem.Data (IDC3)", &Mem_Data_in_ch,
					 2, GBps, 8);
//Hao:change the bandwidth
XBAR_CPL_Mem_Data_pkt_IDC3.SetPipeline(3);
CSimpleTH th_EU_data_to_Mem_Data_IDC3 ("XBAR_CPL: EU.data -> Mem.Data (IDC3)",
				       &XBAR_CPL_EU_data_pkt_IDC3, 
				       &xf_EU_data_to_Mem_Data, 
				       &XBAR_CPL_Mem_Data_pkt_IDC3);



CSource XBAR_CPL_EU_data_pkt_IDC4 ("XBAR_CPL-in: EU.data packet (IDC4)", &IDC4_cpl_ch, is_data_cpl);
CDestination XBAR_CPL_Mem_Data_pkt_IDC4 ("XBAR_CPL-out: Mem.Data (IDC4)", &Mem_Data_in_ch,
					 2, GBps, 8);
//Hao:change the bandwidth
XBAR_CPL_Mem_Data_pkt_IDC4.SetPipeline(3);
CSimpleTH th_EU_data_to_Mem_Data_IDC4 ("XBAR_CPL: EU.data -> Mem.Data (IDC4)",
				       &XBAR_CPL_EU_data_pkt_IDC4, 
				       &xf_EU_data_to_Mem_Data, 
				       &XBAR_CPL_Mem_Data_pkt_IDC4);




CRoundRobinTH th_XBAR_CPL_EU_data_dispatch ("XBAR_CPL: merge EU.data from IDCx to Mem_Data_ch"); //Hao
//CDispatchTH th_XBAR_CPL_EU_data_dispatch ("XBAR_CPL: merge EU.data from IDCx to Mem_Data_ch");//RR? 
th_XBAR_CPL_EU_data_dispatch.AddTH(&th_EU_data_to_Mem_Data_IDC1);
th_XBAR_CPL_EU_data_dispatch.AddTH(&th_EU_data_to_Mem_Data_IDC2);
th_XBAR_CPL_EU_data_dispatch.AddTH(&th_EU_data_to_Mem_Data_IDC3);
th_XBAR_CPL_EU_data_dispatch.AddTH(&th_EU_data_to_Mem_Data_IDC4);


XBAR_CPL.AddTH(&th_XBAR_CPL_EU_data_dispatch);


// ----------------------------------------
//   ODCx: Result.Data -> WR.Data
// ----------------------------------------
F_COND_WITH_TRANSNAME(is_result_data, "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Result.Data");
//CNameTransform xf_rename_to_WR_Data ("rename Result.Data to WR.Data", "WR.Data");
//CSplitTransform xf_split_wr_128 ("Split WR.Data onSIZE 128", onSIZE, 128);
//CGroupTransform xf_Result_Data_to_WR_Data ("Result.Data to WR.Data", 1);
//xf_Result_Data_to_WR_Data.AddTransform(&xf_rename_to_WR_Data);
//xf_Result_Data_to_WR_Data.AddTransform(&xf_split_wr_128);
CNameTransform xf_Result_Data_to_WR_Data ("rename Result.Data to WR.Data", "WR.Data");

CSource DMA_ODC1_Result_Data_pkt ("DMA_ODC1-in: Result.Data packet", &ODC1_req_ch, is_result_data);
CDestination DMA_ODC1_WR_Data_pkt ("DMA_ODC1-out: WR.Data packet", &ODC1_wr_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_result_data_to_wr_data_ODC1 ("DMA_ODC1: Result.Data -> WR.Data (ODC1)",
					  &DMA_ODC1_Result_Data_pkt,
					  &xf_Result_Data_to_WR_Data,
					  &DMA_ODC1_WR_Data_pkt);
DMA_ODC1.AddTH(&th_result_data_to_wr_data_ODC1);



CSource DMA_ODC2_Result_Data_pkt ("DMA_ODC2-in: Result.Data packet", &ODC2_req_ch, is_result_data);
CDestination DMA_ODC2_WR_Data_pkt ("DMA_ODC2-out: WR.Data packet", &ODC2_wr_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_result_data_to_wr_data_ODC2 ("DMA_ODC2: Result.Data -> WR.Data (ODC2)",
					  &DMA_ODC2_Result_Data_pkt,
					  &xf_Result_Data_to_WR_Data,
					  &DMA_ODC2_WR_Data_pkt);
DMA_ODC2.AddTH(&th_result_data_to_wr_data_ODC2);



CSource DMA_ODC3_Result_Data_pkt ("DMA_ODC3-in: Result.Data packet", &ODC3_req_ch, is_result_data);
CDestination DMA_ODC3_WR_Data_pkt ("DMA_ODC3-out: WR.Data packet", &ODC3_wr_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_result_data_to_wr_data_ODC3 ("DMA_ODC3: Result.Data -> WR.Data (ODC3)",
					  &DMA_ODC3_Result_Data_pkt,
					  &xf_Result_Data_to_WR_Data,
					  &DMA_ODC3_WR_Data_pkt);
DMA_ODC3.AddTH(&th_result_data_to_wr_data_ODC3);


CSource DMA_ODC4_Result_Data_pkt ("DMA_ODC4-in: Result.Data packet", &ODC4_req_ch, is_result_data);
CDestination DMA_ODC4_WR_Data_pkt ("DMA_ODC4-out: WR.Data packet", &ODC4_wr_ch,
				   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_result_data_to_wr_data_ODC4 ("DMA_ODC4: Result.Data -> WR.Data (ODC4)",
					  &DMA_ODC4_Result_Data_pkt,
					  &xf_Result_Data_to_WR_Data,
					  &DMA_ODC4_WR_Data_pkt);
DMA_ODC4.AddTH(&th_result_data_to_wr_data_ODC4);


//----------------------------------------
//Hao: CPLS: Result.Desc -> WR.Desc
//----------------------------------------
F_COND_WITH_TRANSNAME(is_result_desc, "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Result.Desc");
CNameTransform xf_rename_to_WR_Desc ("rename Result.Desc to WR.Desc", "WR.Desc");
CSplitTransform xf_split_wr_desc_128 ("Split WR.Desc onSIZE 128", onSIZE, 128);
CGroupTransform xf_Result_Desc_to_WR_Desc ("Result.Desc to WR.Desc", 1);
xf_Result_Desc_to_WR_Desc.AddTransform(&xf_rename_to_WR_Desc);
xf_Result_Desc_to_WR_Desc.AddTransform(&xf_split_wr_desc_128);


CSource DMA_CPLS_Result_Desc_pkt ("DMA_CPLS-in: Result.Desc packet", &CPLS_req_ch, is_result_desc);
CDestination DMA_CPLS_WR_Desc_pkt ("DMA_CPLS-out: WR.Desc packet", &CPLS_wr_ch,
				   2, GBps, 8);
CSimpleTH th_result_desc_to_wr_desc_CPLS ("DMA_CPLS: Result.Desc -> WR.Desc (CPLS)",
					  &DMA_CPLS_Result_Desc_pkt,
					  &xf_Result_Desc_to_WR_Desc,
					  &DMA_CPLS_WR_Desc_pkt);
DMA_CPLS.AddTH(&th_result_desc_to_wr_desc_CPLS);

//Hao:end


// ----------------------------------------
//   DMA_WRC: WR.Data -> PCIE.WR.Data
// ----------------------------------------
F_COND_WITH_TRANSNAME (is_wr_data, "DMA Loopback Transaction", 
		       PIPELINE, PacketName == "WR.Data");
CNameTransform xf_WR_Data_to_PCIE_WR_Data ("WR.Data to PCIE.WR.Data (ODC1)", "PCIE.WR.Data");


CSource DMA_WRC_WR_Data_pkt_ODC1 ("DMA_WRC-in: WR.Data (ODC1)", &ODC1_wr_ch, is_wr_data);
CDestination DMA_WRC_PCIE_WR_Data_pkt_ODC1 ("DMA_WR-out: PCIE.WR.Data packet (ODC1)", &PCIE_Egress_ch,
					    2, GBps, 8);
//Hao:change the bandwidth
DMA_WRC_PCIE_WR_Data_pkt_ODC1.SetPipeline(18);
//Hao:add internal delay to WRC->PCIE
CSimpleTH th_WR_Data_to_PCIE_WR_Data_ODC1 ("DMA_WR: WR.Data -> PCIE.WR.Data (ODC1)",
					   &DMA_WRC_WR_Data_pkt_ODC1, 
					   &xf_WR_Data_to_PCIE_WR_Data, 
					   &DMA_WRC_PCIE_WR_Data_pkt_ODC1);



CSource DMA_WRC_WR_Data_pkt_ODC2 ("DMA_WRC-in: WR.Data (ODC2)", &ODC2_wr_ch, is_wr_data);
CDestination DMA_WRC_PCIE_WR_Data_pkt_ODC2 ("DMA_WR-out: PCIE.WR.Data packet (ODC2)", &PCIE_Egress_ch,
					    2, GBps, 8);
//Hao:change the bandwidth
DMA_WRC_PCIE_WR_Data_pkt_ODC2.SetPipeline(18);
//Hao:add internal delay to WRC->PCIE
CSimpleTH th_WR_Data_to_PCIE_WR_Data_ODC2 ("DMA_WR: WR.Data -> PCIE.WR.Data (ODC2)",
					   &DMA_WRC_WR_Data_pkt_ODC2, 
					   &xf_WR_Data_to_PCIE_WR_Data, 
					   &DMA_WRC_PCIE_WR_Data_pkt_ODC2);


CSource DMA_WRC_WR_Data_pkt_ODC3 ("DMA_WRC-in: WR.Data (ODC3)", &ODC3_wr_ch, is_wr_data);
CDestination DMA_WRC_PCIE_WR_Data_pkt_ODC3 ("DMA_WR-out: PCIE.WR.Data packet (ODC3)", &PCIE_Egress_ch,
					    2, GBps, 8);
//Hao:change the bandwidth
DMA_WRC_PCIE_WR_Data_pkt_ODC3.SetPipeline(18);
//Hao:add internal delay to WRC->PCIE
CSimpleTH th_WR_Data_to_PCIE_WR_Data_ODC3 ("DMA_WR: WR.Data -> PCIE.WR.Data (ODC3)",
					   &DMA_WRC_WR_Data_pkt_ODC3, 
					   &xf_WR_Data_to_PCIE_WR_Data, 
					   &DMA_WRC_PCIE_WR_Data_pkt_ODC3);



CSource DMA_WRC_WR_Data_pkt_ODC4 ("DMA_WRC-in: WR.Data (ODC4)", &ODC4_wr_ch, is_wr_data);
CDestination DMA_WRC_PCIE_WR_Data_pkt_ODC4 ("DMA_WR-out: PCIE.WR.Data packet (ODC4)", &PCIE_Egress_ch,
					    2, GBps, 8);
//Hao:change the bandwidth
DMA_WRC_PCIE_WR_Data_pkt_ODC4.SetPipeline(18);
//Hao:add internal delay to WRC->PCIE
CSimpleTH th_WR_Data_to_PCIE_WR_Data_ODC4 ("DMA_WR: WR.Data -> PCIE.WR.Data (ODC4)",
					   &DMA_WRC_WR_Data_pkt_ODC4, 
					   &xf_WR_Data_to_PCIE_WR_Data, 
					   &DMA_WRC_PCIE_WR_Data_pkt_ODC4);

//----------------------------------------
//Hao: DMA_WRC: WR.Desc -> PCIE.WR.Desc
//----------------------------------------
F_COND_WITH_TRANSNAME (is_wr_desc, "DMA Loopback Transaction", 
		       PIPELINE, PacketName == "WR.Desc");
CNameTransform xf_WR_Desc_to_PCIE_WR_Desc ("WR.Desc to PCIE.WR.Desc (CPLS)", "PCIE.WR.Desc");
CSource DMA_WRC_WR_Desc_pkt_CPLS ("DMA_WRC-in: WR.Desc (CPLS)", &CPLS_wr_ch, is_wr_desc);
CDestination DMA_WRC_PCIE_WR_Desc_pkt_CPLS ("DMA_WR-out: PCIE.WR.Desc packet (CPLS)", &PCIE_Egress_ch,
					    2, GBps, 8);
DMA_WRC_PCIE_WR_Desc_pkt_CPLS.SetPipeline(18);
//Hao:add internal delay to WRC->PCIE
CSimpleTH th_WR_Desc_to_PCIE_WR_Desc_CPLS ("DMA_WR: WR.Desc -> PCIE.WR.Desc (CPLS)",
					   &DMA_WRC_WR_Desc_pkt_CPLS, 
					   &xf_WR_Desc_to_PCIE_WR_Desc, 
					   &DMA_WRC_PCIE_WR_Desc_pkt_CPLS);

//Hao:end


CRoundRobinTH th_wr_result_to_host ("DMA_WRC: RoundRobin result PCIE.WR.Data");
th_wr_result_to_host.AddTH(&th_WR_Data_to_PCIE_WR_Data_ODC1);
th_wr_result_to_host.AddTH(&th_WR_Data_to_PCIE_WR_Data_ODC2);
th_wr_result_to_host.AddTH(&th_WR_Data_to_PCIE_WR_Data_ODC3);
th_wr_result_to_host.AddTH(&th_WR_Data_to_PCIE_WR_Data_ODC4);
th_wr_result_to_host.AddTH(&th_WR_Desc_to_PCIE_WR_Desc_CPLS);
//Hao: add the write desc flow

DMA_WRC.AddTH(&th_wr_result_to_host);

//********************************************************************************
//Hao: open point traffic
//********************************************************************************

// ----------------------------------------
//   XBAR_REQ
//   EU.cmd -> Data.fetch.req
// ----------------------------------------
F_COND_WITH_TRANSNAME(is_eu_cmd, "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "EU.cmd");


CNameTransform xf_rename_to_Data_fetch_req ("rename EU.cmd to Data.fetch.req", "Data.fetch.req");
//CLengthTransform xf_random_length_Data_fetch ("set random length in Data.fetch.req", 128, 250, 2);
CSizeTransform xf_resize_to_Data_fetch_req ("resize EU.cmd to Data.fetch.req", 16);
CGroupTransform xf_EU_cmd_to_Data_fetch_req ("EU.cmd to Data.fetch.req", 1);
xf_EU_cmd_to_Data_fetch_req.AddTransform(&xf_rename_to_Data_fetch_req);
xf_EU_cmd_to_Data_fetch_req.AddTransform(&xf_resize_to_Data_fetch_req);


CSource XBAR_REQ_EU_cmd_pkt_IDC1 ("XBAR_REQ-in: EU.cmd packet (IDC1)", 
				  &CMD_fwd_ch, is_eu_cmd);
CDestination XBAR_REQ_Data_fetch_req_pkt_IDC1 ("XBAR_REQ-out: Data.fetch.req packet (IDC1)", &IDC1_req_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_EU_cmd_to_Data_fetch_req_IDC1 ("XBAR_REQ: EU.cmd -> Data.fetch.req (IDC1)",
					    &XBAR_REQ_EU_cmd_pkt_IDC1, 
					    &xf_EU_cmd_to_Data_fetch_req, 
					    &XBAR_REQ_Data_fetch_req_pkt_IDC1);




CSource XBAR_REQ_EU_cmd_pkt_IDC2 ("XBAR_REQ-in: EU.cmd packet (IDC2)", 
				  &CMD_fwd_ch, is_eu_cmd);
CDestination XBAR_REQ_Data_fetch_req_pkt_IDC2 ("XBAR_REQ-out: Data.fetch.req packet (IDC2)", &IDC2_req_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_EU_cmd_to_Data_fetch_req_IDC2 ("XBAR_REQ: EU.cmd -> Data.fetch.req (IDC2)",
					    &XBAR_REQ_EU_cmd_pkt_IDC2, 
					    &xf_EU_cmd_to_Data_fetch_req, 
					    &XBAR_REQ_Data_fetch_req_pkt_IDC2);



CSource XBAR_REQ_EU_cmd_pkt_IDC3 ("XBAR_REQ-in: EU.cmd packet (IDC3)", 
				  &CMD_fwd_ch, is_eu_cmd);
CDestination XBAR_REQ_Data_fetch_req_pkt_IDC3 ("XBAR_REQ-out: Data.fetch.req packet (IDC3)", &IDC3_req_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_EU_cmd_to_Data_fetch_req_IDC3 ("XBAR_REQ: EU.cmd -> Data.fetch.req (IDC3)",
					    &XBAR_REQ_EU_cmd_pkt_IDC3, 
					    &xf_EU_cmd_to_Data_fetch_req, 
					    &XBAR_REQ_Data_fetch_req_pkt_IDC3);


CSource XBAR_REQ_EU_cmd_pkt_IDC4 ("XBAR_REQ-in: EU.cmd packet (IDC4)", 
				  &CMD_fwd_ch, is_eu_cmd);
CDestination XBAR_REQ_Data_fetch_req_pkt_IDC4 ("XBAR_REQ-out: Data.fetch.req packet (IDC4)", &IDC4_req_ch,
					       2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_EU_cmd_to_Data_fetch_req_IDC4 ("XBAR_REQ: EU.cmd -> Data.fetch.req (IDC4)",
					    &XBAR_REQ_EU_cmd_pkt_IDC4, 
					    &xf_EU_cmd_to_Data_fetch_req, 
					    &XBAR_REQ_Data_fetch_req_pkt_IDC4);



CDispatchTH th_XBAR_REQ_dispatch_EU_cmd ("XBAR_REQ: Dispatch EU.cmd");
th_XBAR_REQ_dispatch_EU_cmd.AddTH(&th_EU_cmd_to_Data_fetch_req_IDC1);
th_XBAR_REQ_dispatch_EU_cmd.AddTH(&th_EU_cmd_to_Data_fetch_req_IDC2);
th_XBAR_REQ_dispatch_EU_cmd.AddTH(&th_EU_cmd_to_Data_fetch_req_IDC3);
th_XBAR_REQ_dispatch_EU_cmd.AddTH(&th_EU_cmd_to_Data_fetch_req_IDC4);

XBAR_REQ.AddTH(&th_XBAR_REQ_dispatch_EU_cmd);

//----------------------------------------
// Hao: Transform from Mem.Data
// Mem.Data  -> Mem.Desc
//           -> Mem.Data.Out
//----------------------------------------
F_COND_WITH_TRANSNAME(is_mem_data,  "DMA Loopback Transaction", 
		      PIPELINE, PacketName == "Mem.Data");
CNameTransform xf_Mem_Data_to_Mem_Data_Out ("Mem.Data to Mem.Data.Out", "Mem.Data.Out");

CNameTransform xf_Mem_Data_to_Mem_Desc_Name ("Mem.Data to Mem.Desc", "Mem.Desc");
CSizeTransform xf_Mem_Data_to_Mem_Desc_Size ("Mem.Data to Mem.Desc", 16);//Hao: each result descriptor is 16Byte
CGroupTransform xf_Mem_Data_to_Mem_Desc ("Mem.Data to Mem.Desc", 1);
xf_Mem_Data_to_Mem_Desc.AddTransform(&xf_Mem_Data_to_Mem_Desc_Name);
xf_Mem_Data_to_Mem_Desc.AddTransform(&xf_Mem_Data_to_Mem_Desc_Size);

CSource EUx_Mem_data_pkt("EUx-in: Mem.Data packet", &Mem_Data_in_ch, is_mem_data);
CDestination EUx_Mem_data_out_pkt("EUx-out: Mem.Data.Out",&Mem_Data_out_ch,
				  2, GBps, 8);
EUx_Mem_data_out_pkt.SetPipeline(3);
CDestination EUx_Mem_data_out_desc_pkt("EUx-out: Mem.Desc",&Mem_CTL_ch,
				       2, GBps, 8);
EUx_Mem_data_out_desc_pkt.SetPipeline(3);
CSimpleTH th_EUx_Mem_Data_to_DataDesc_Out("EUx: Mem.Data -> Mem.Data.Out",
					  &EUx_Mem_data_pkt,
					  &xf_Mem_Data_to_Mem_Data_Out,
					  &EUx_Mem_data_out_pkt);
th_EUx_Mem_Data_to_DataDesc_Out.AddLogicCone(&EUx_Mem_data_pkt,
					     &xf_Mem_Data_to_Mem_Desc,
					     &EUx_Mem_data_out_desc_pkt
					     );
EUx.AddTH(&th_EUx_Mem_Data_to_DataDesc_Out);

// ----------------------------------------
//  XBAR_WR: Mem.Data.Out -> ODCx Result.Data
//Hao: add one more path
// ----------------------------------------
F_COND_WITH_TRANSNAME(is_mem_data_out,  "DMA Loopback Transaction", 
		      SERIAL_AHEAD, PacketName == "Mem.Data.Out");
CNameTransform xf_Mem_Data_to_Result_Data ("Mem.Data.Out to Result.Data", "Result.Data");



CSource XBAR_WR_Mem_Data_pkt_ODC1 ("XBAR_WR-in: Mem.Data packet (ODC1)", &Mem_Data_out_ch, is_mem_data_out);
CDestination XBAR_WR_Result_Data_pkt_ODC1 ("XBAR_WR-out: Result.Data (ODC1)", &ODC1_req_ch,
					   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_Mem_Data_to_Result_Data_ODC1 ("XBAR_WR: Mem.Data -> Result.Data (ODC1)",
					   &XBAR_WR_Mem_Data_pkt_ODC1, 
					   &xf_Mem_Data_to_Result_Data, 
					   &XBAR_WR_Result_Data_pkt_ODC1);

CSource XBAR_WR_Mem_Data_pkt_ODC2 ("XBAR_WR-in: Mem.Data packet (ODC2)", &Mem_Data_out_ch, is_mem_data_out);
CDestination XBAR_WR_Result_Data_pkt_ODC2 ("XBAR_WR-out: Result.Data (ODC2)", &ODC2_req_ch,
					   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_Mem_Data_to_Result_Data_ODC2 ("XBAR_WR: Mem.Data -> Result.Data (ODC2)",
					   &XBAR_WR_Mem_Data_pkt_ODC2, 
					   &xf_Mem_Data_to_Result_Data, 
					   &XBAR_WR_Result_Data_pkt_ODC2);


CSource XBAR_WR_Mem_Data_pkt_ODC3 ("XBAR_WR-in: Mem.Data packet (ODC3)", &Mem_Data_out_ch, is_mem_data_out);
CDestination XBAR_WR_Result_Data_pkt_ODC3 ("XBAR_WR-out: Result.Data (ODC3)", &ODC3_req_ch,
					   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_Mem_Data_to_Result_Data_ODC3 ("XBAR_WR: Mem.Data -> Result.Data (ODC3)",
					   &XBAR_WR_Mem_Data_pkt_ODC3, 
					   &xf_Mem_Data_to_Result_Data, 
					   &XBAR_WR_Result_Data_pkt_ODC3);


CSource XBAR_WR_Mem_Data_pkt_ODC4 ("XBAR_WR-in: Mem.Data packet (ODC4)", &Mem_Data_out_ch, is_mem_data_out);
CDestination XBAR_WR_Result_Data_pkt_ODC4 ("XBAR_WR-out: Result.Data (ODC4)", &ODC4_req_ch,
					   2, GBps, 8);
//Hao:change the bandwidth
CSimpleTH th_Mem_Data_to_Result_Data_ODC4 ("XBAR_WR: Mem.Data -> Result.Data (ODC4)",
					   &XBAR_WR_Mem_Data_pkt_ODC4, 
					   &xf_Mem_Data_to_Result_Data, 
					   &XBAR_WR_Result_Data_pkt_ODC4);

CDispatchTH th_result_data_gen_dispatch ("XBAR_WR: dispatch Mem.Data to ODCx Result.Data or CPLS Result.Desc");
th_result_data_gen_dispatch.AddTH(&th_Mem_Data_to_Result_Data_ODC1);
th_result_data_gen_dispatch.AddTH(&th_Mem_Data_to_Result_Data_ODC2);
th_result_data_gen_dispatch.AddTH(&th_Mem_Data_to_Result_Data_ODC3);
th_result_data_gen_dispatch.AddTH(&th_Mem_Data_to_Result_Data_ODC4);

XBAR_WR.AddTH(&th_result_data_gen_dispatch);

//----------------------------------------
//Hao: Mem.Desc -> result.desc
//----------------------------------------
F_COND_WITH_TRANSNAME (is_mem_desc, "DMA Loopback Transaction", 
		       PIPELINE, PacketName == "Mem.Desc");
CNameTransform xf_Mem_desc_to_Result_desc("Mem.Desc to Result.Desc", "Result.Desc");
CSource XBAR_REQ_Mem_desc("XBAR_REQ-in:Mem.Desc packet", &Mem_CTL_ch, is_mem_desc);
CDestination XBAR_REQ_Result_Desc("XBAR_REQ-out: Result.Desc", &CPLS_req_ch,
				  2, GBps, 8);
CSimpleTH th_Mem_desc_to_result_desc("XBAR_REQ: Mem.Desc -> Result.Desc",
				     &XBAR_REQ_Mem_desc,
				     &xf_Mem_desc_to_Result_desc,
				     &XBAR_REQ_Result_Desc);
XBAR_REQ.AddTH(&th_Mem_desc_to_result_desc); //Hao: add this flow
//Hao:end



/* //---------------------------------------- */
/* //Hao: Mem.data -> Mem.Desc */
/* //---------------------------------------- */
/* F_COND_WITH_TRANSNAME(is_mem_data,  "DMA Loopback Transaction",  */
/* 		      PIPELINE, PacketName == "Mem.Data"); */
/* CNameTransform xf_Mem_Data_to_Mem_Desc("Mem.Data to Mem.Desc", "Mem.Desc"); */
/* CSource EUx_Mem_Data_pkt_CPLS ("EUx-in: Mem.Data packet (CPLS)", &Mem_Data_in_ch, is_mem_data); */
/* CDestination EUx_Result_Desc_pkt_CPLS ("EUx-out: Result.Desc (CPLS)", &Mem_CTL_ch, */
/* 					   2, GBps, 8); */
/* CSimpleTH th_EUx_Mem_Data_to_Result_Desc_CPLS ("EUx: Mem.Data -> Result.Desc (CPLS)", */
/* 					       &EUx_Mem_Data_pkt_CPLS,  */
/* 					       &xf_Mem_Data_to_Mem_Desc, */
/* 					       &EUx_Result_Desc_pkt_CPLS); */

/* //---------------------------------------- */
/* //Hao: EUx: Mem.Data -> Mem.Data.Out */
/* //---------------------------------------- */
/* F_COND_WITH_TRANSNAME (is_eux_data, "DMA Loopback Transaction",  */
/* 		       PIPELINE, PacketName == "Mem.Data"); */
/* CNameTransform xf_EU_Mem_data_to_Mem_Data_Out ("Mem.Data to Mem.Data.Out", "Mem.Data.Out"); */
/* CSource EUx_Mem_data_pkt("EUx-in: Mem.Data packet", &Mem_Data_in_ch, is_eux_data); */
/* CDestination EUx_Mem_data_out_pkt("EUx-out: Mem.Data.Out",&Mem_Data_out_ch, */
/* 				  2, GBps, 8); */
/* EUx_Mem_data_out_pkt.SetPipeline(3); */
/* CSimpleTH th_EUx_Mem_Data_to_Mem_Data_Out("EUx: Mem.Data -> Mem.Data.Out", */
/* 					  &EUx_Mem_data_pkt, */
/* 					  &xf_EU_Mem_data_to_Mem_Data_Out, */
/* 					  &EUx_Mem_data_out_pkt); */


/* CDispatchTH th_EUx_dispatch("EUx:Dispatch Mem.Data to CPLS or ODCx"); */
/* th_EUx_dispatch.AddTH(&th_EUx_Mem_Data_to_Result_Desc_CPLS); */
/* th_EUx_dispatch.AddTH(&th_EUx_Mem_Data_to_Mem_Data_Out); */
/* EUx.AddTH(&th_EUx_dispatch); */
/* //EUx.AddTH(&th_EUx_Mem_Data_to_Mem_Data_Out); */
/* //EUx.AddTH(&th_EUx_Mem_Data_to_Result_Desc_CPLS); */


//Hao:end

