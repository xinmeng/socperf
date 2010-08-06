
//----------------------------------------
//Hao: add this section for CP fetch
//----------------------------------------
//Hao: DMA.CPP: CPR.Fetch.req -> CPR.fetch
F_COND_WITH_TRANSNAME(is_cpr_fetch_req, "DMA Loopback Transaction",
			 SERIAL_AHEAD, PacketName == "CPR.fetch.req");
CSource DMA_CPP_CPR_fetch_req_pkt ("DMA.CPP-in: CPR.fetch.req packet",
				   &CPR_req_ch,is_cpr_fetch_req); //use the individual channel
CNameTransform xf_CPR_fetch_req_2_CPR_fetch ("CPR.fetch.req to CPR.fetch","CPR.fetch");
CDestination DMA_CPP_CPR_fetch_pkt("DMA.CPP-out: CPR.fetch packet", &CMD_fetch_ch, 1, GBps, 8);
DMA_CPP_CPR_fetch_pkt.SetPacketDelay(800);
DMA_CPP_CPR_fetch_pkt.SetPipeline(3);

CSimpleTH th_CPR_fetch_req_2_CPR_fetch ("DMA.CPP: CPR.fetch.req -> CPR.fetch",
					&DMA_CPP_CPR_fetch_req_pkt, &xf_CPR_fetch_req_2_CPR_fetch, &DMA_CPP_CPR_fetch_pkt);
//Hao: this simple TH which will be mereged as RoundRobin TH





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
				    1, GBps, 8);
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




/* generate stimulous */
CPacketFactory CPR_fetch_req_factory ("CPR.fetch.req");
CPR_fetch_req_factory.SetLength(64); //Hao:32:64/64:128 but size transfer at the completion
CPR_fetch_req_factory.SetSize(16);  
CPR_fetch_req_factory.SetCount(10); //Hao: the number of stimulus packets
//CPR_fetch_req_factory.SetDelay(800);//Hao: add the delay for CPR request

/* generate stimulous */
CPacketFactory CMD_fetch_req_factory ("CMD.fetch.req");
CMD_fetch_req_factory.SetLength(64); //Hao:32:64/64:128 but size transfer at the completion
CMD_fetch_req_factory.SetSize(16);  
CMD_fetch_req_factory.SetCount(10); //Hao: the number of stimulus packets
//CMD_fetch_req_factory.SetDelay(800);//Hao: add the delay for CMD request


CPMTransaction *dma_loopback_trans = TransactionFactory.NewTransaction("DMA Loopback Transaction");
DMA_CPP_CPR_fetch_req_pkt.BackDoorWrite(dma_loopback_trans, CPR_fetch_req_factory.GenPacket());
DMA_CPP_CMD_fetch_req_pkt.BackDoorWrite(dma_loopback_trans, CMD_fetch_req_factory.GenPacket());
