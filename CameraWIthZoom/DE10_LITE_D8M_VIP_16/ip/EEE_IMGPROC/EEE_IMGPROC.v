module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode
	
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode;

////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter CROSSHAIR_L = 11'd300;
parameter CROSSHAIR_R = 11'd340;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;


wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;
reg  [2:0]   Colour;

wire         sop, eop, in_valid, out_ready;





////////////////////////////////////////////////////////////////////////

//CONVERTING RGB TO HSV

////////////////////////////////////////////////////////////////////////

//Make R, G and B 15 bits
wire signed [15:0] R;
wire signed [15:0] G;
wire signed [15:0] B;
		
assign R = {8'b0, red};
assign G = {8'b0, green};
assign B = {8'b0, blue};
		
//State other variables
reg signed [15:0] S, V;
reg signed [15:0] H;
reg signed [15:0] max, min;

reg signed [15:0] dif;
		
		
//Finding the max, min and difference between max and min
always @ (posedge clk) begin
	if 		(R > G && R > B) 	max <= R;
	else if 	(G > B)			 	max <= G;
	else 								max <= B;   

	if 		(R < G && R < B) 	min <= R;
	else if 	(G < B)			 	min <= G;
	else 								min <= B; 

	dif <= max - min;
	
	//S
	if (max == 0) 	S <= 0;
	else 				S <= (dif*100)/max;		
	
	//V
	V <= (max*100)/255;
	
	//H
	if 		(dif == 0) 			H <= 0;
	else if 	(R > G && R > B) 	H <= ((G - B) * 60)/dif;
	else if 	(G > B)			 	H <= ((B - R) * 60)/dif + 120;	
	else 								H <= ((R - G) * 60)/dif + 240;
	
	//if 		(V >= 100 && S <= 25) 								 Colour <= 3'b011; //White
	if	(V >= 40 && S >= 70 && H <= 240 && H >= 180) 		 Colour <= 3'b010; //Blue		//V >= 89
	else if	(V >= 70 && S >= 70 && H <= 72 && H >= 30) 	 Colour <= 3'b001; //Yellow	41 and 71 and 20 min hue
	else if	(V >= 50 && S >= 50 && H <= 20 && H >= -20)   Colour <= 3'b000; //Red	//V >= 90
	else  																 Colour <= 3'b100; //Grey
	
end

/*
reg signed [7:0] S, V;
reg signed [7:0] H;
reg signed [7:0] max, min;

reg signed [8:0] dif;

always @ (*) begin
	if 		(R > G && R > B) 	max <= R;
	else if 	(G > B && G > R)	max <= G;
	else 								max <= B;   

	if 		(R < G && R < B) 	min <= R;
	else if 	(G < B && G < R)  min <= G;
	else 								min <= B; 

	dif <= max - min;
	
	//S
	if (max == 0) 	S <= 0;
	else 				S <= (dif*100)/max;		
	
	//V
	V <= (max*100)/255;
	
	//H
	if 		(dif == 0) 			H <= 0;
	else if 	(R > G && R > B) 	H <= ((G - B) * 60)/dif;
	else if 	(G > B)			 	H <= ((B - R) * 60)/dif + 120;	
	else 								H <= ((R - G) * 60)/dif + 240;
	
	if 		(V >= 100 && S <= 25) 								 Colour <= 3'b011; //White
	if	(V >= 100 && S >= 71 && H <= 240 && H >= 180) 		 Colour <= 3'b010; //Blue		//V >= 89
	else if	(V >= 100 && S >= 71 && H <= 72 && H >= 30) 	 Colour <= 3'b001; //Yellow	41 and 71 and 20 min hue
	else if	(V >= 100 && S >= 71 && H <= 20 && H >= -20)  Colour <= 3'b000; //Red	//V >= 90
	else  																 Colour <= 3'b100; //Grey
	
end
*/

////////////////////////////////////////////////////////////////////////

//CONVERTING RGB TO HSV

////////////////////////////////////////////////////////////////////////
		
		
/*
wire white_detect;
assign white_detect = (red[7:4] == 4'hf) && (green[7:4] == 4'hf) && (blue[7:4] == 4'hf);	

wire [23:0] white_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign white_high = white_detect ? {8'hff, 8'hff, 8'h00} : {grey, grey, grey};	
*/
		
				
// Highlight detected areas
reg [23:0] high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4

always @ (posedge clk) begin	//Write words to FIFO as state machine advances
	case(Colour)
		3'b000: begin
			high  <=  {8'hff, 8'h00, 8'h00};	//red
		end
		3'b001: begin
			high  <=  {8'hff, 8'hff, 8'h00};	//yellow
		end
		3'b010: begin
			high  <=  {8'h00, 8'h00, 8'hff};	//blue
		end
		3'b100: begin
			high  <=  {grey, grey, grey};		//grey
		end
		default: begin
			high  <=  {grey, grey, grey};		//grey
		end
	endcase
end

// Show bounding box
/*
(x_min,y_min) = = = (x_max,y_min)
      =						  =
      =						  =
      =						  =
(x_min,y_max) = = = (x_max,y_max)
*/

reg [23:0] new_image;
wire bb_active_R, bb_active_Y, bb_active_B, crosshair;
assign bb_active_R = (x == left_R) | (x == right_R) | (y == top_R) | (y == bottom_R); 
assign bb_active_Y = (x == left_Y) | (x == right_Y) | (y == top_Y) | (y == bottom_Y); 
assign bb_active_B = (x == left_B) | (x == right_B) | (y == top_B) | (y == bottom_B);
assign crosshair   = (x == CROSSHAIR_L) | (x == CROSSHAIR_R);

always @ (*) begin
	if (bb_active_R) 			new_image <= {8'hff, 8'h00, 8'h00};
	else if (bb_active_Y) begin
		if	(right_Y < right_R && top_Y > top_R) new_image <= high;
		else 												 new_image <= {8'hff, 8'hff, 8'h00};
	end
	else if (bb_active_B) 	new_image <= {8'h00, 8'h00, 8'hff};
	else if (crosshair)		new_image <= {8'h00, 8'hff, 8'h00};
	else 							new_image <= high;
end

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
	if (sop) begin
		x <= 11'h0;
		y <= 11'h0;
		packet_video <= (blue[3:0] == 3'h0);
	end
	else if (in_valid) begin
		if (x == IMAGE_W-1) begin
			x <= 11'h0;
			y <= y + 11'h1;
		end
		else begin
			x <= x + 11'h1;
		end
	end
end

//Different current min and max values/coordinates for bounding boxes of each colour
reg [10:0] R_x_min, R_y_min, R_x_max, R_y_max;
reg [10:0] Y_x_min, Y_y_min, Y_x_max, Y_y_max;
reg [10:0] B_x_min, B_y_min, B_x_max, B_y_max;
reg [7:0]  R_found, Y_found, B_found;
reg [10:0] average_R, average_Y, average_B;
reg [15:0] R_BB_Height, Y_BB_Height, B_BB_Height;
reg [7:0] Zoom;
reg R_Zoom, Y_Zoom, B_Zoom;
reg [1:0] Test;

always@(posedge clk) begin
	if ((Colour == 3'b000) & in_valid) begin	//Update bounds when the pixel is red
		if (x < R_x_min) R_x_min <= x;
		if (x > R_x_max) R_x_max <= x;
		if (y < R_y_min) R_y_min <= y;
		R_y_max <= y;
	end
	
	if ((Colour == 3'b001) & in_valid) begin	//Update bounds when the pixel is yellow
		if (x < Y_x_min) Y_x_min <= x;
		if (x > Y_x_max) Y_x_max <= x;
		if (y < Y_y_min) Y_y_min <= y;
		Y_y_max <= y;
	end
	
	if ((Colour == 3'b010) & in_valid) begin	//Update bounds when the pixel is blue
		if (x < B_x_min) B_x_min <= x;
		if (x > B_x_max) B_x_max <= x;
		if (y < B_y_min) B_y_min <= y;
		B_y_max <= y;
	end
	
	average_R <= (R_x_max + R_x_min)/2;
	average_Y <= (Y_x_max + Y_x_min)/2;
	average_B <= (B_x_max + B_x_min)/2;

	//Sending the bounding box height
	if (R_y_max == 0) R_BB_Height <= 0;
	else 					R_BB_Height <= {5'b0, R_y_max - R_y_min};
	if (Y_y_max == 0) Y_BB_Height <= 0;
	else 					Y_BB_Height <= {5'b0, Y_y_max - Y_y_min};
	if (B_y_max == 0) B_BB_Height <= 0;
	else 				   B_BB_Height <= {5'b0, B_y_max - B_y_min};
	
	/*
	if (Zoom == 0
		&& (average_R >= CROSSHAIR_L && average_R <= CROSSHAIR_R && R_x_max != 0 && R_y_max - R_y_min < 40) 
		|| (average_Y >= CROSSHAIR_L && average_Y <= CROSSHAIR_R && Y_x_max != 0 && (right_Y >= right_R && top_Y <= top_R) && Y_y_max - Y_y_min < 40)
		|| (average_B >= CROSSHAIR_L && average_B <= CROSSHAIR_R && B_x_max != 0 && B_y_max - B_y_min < 40))
			Zoom <= 8'h01;
	else if (Zoom == 1 && ) Zoom <= 8'h00;
	*/
	
	
	//////////////////////////////////////////////////////////////////////////////////////
	//Checking if the beacons are in the middle of the screen and autozoom functionality//
	//////////////////////////////////////////////////////////////////////////////////////
	
	//R_Zoom == 1 && right_R > 300 && left_R < 340
	
	/*
	//Red Detection
	if (R_Zoom == 0 && average_R >= CROSSHAIR_L && average_R <= CROSSHAIR_R) begin //Beacon spotted
		R_found <= 8'h01;																				 //With no zoom
		if (R_y_max - R_y_min < 40) R_Zoom <= 1;
		else R_Zoom <= 0;
	end
	else if (R_Zoom == 1 && R_x_max > CROSSHAIR_L && R_x_min < CROSSHAIR_R) begin	//This will check after beacon
		R_found <= 8'h01;																				//spotted and zoomed in
		R_Zoom <= 1;
	end
	else begin																							//Beacon not spotted
		R_found <= 8'h00;
		R_Zoom <= 0;
	end
	

	
	//Yellow Detection
	if (average_Y < CROSSHAIR_L || average_Y > CROSSHAIR_R || Y_x_max == 0 || 
		(right_Y < right_R && top_Y > top_R)) begin
		Y_found <= 8'h00;
		Y_Zoom <= 0;
	end
	else if (Y_Zoom == 1 && Y_y_max > CROSSHAIR_L && Y_y_min < CROSSHAIR_R) begin
		Y_found <= 8'h01;																				
		Y_Zoom <= 1;
	end
	else begin
		Y_found <= 8'h01;
		if (Y_y_max - Y_y_min < 40 && Y_Zoom == 0) Y_Zoom <= 1;
	end
	*/
	
	//Red Detection
	if (Zoom == 0 && average_R >= CROSSHAIR_L && average_R <= CROSSHAIR_R && R_x_max != 0) begin //Beacon spotted																	  						//With no zoom
		R_found <= 8'h01;
		if (R_y_max - R_y_min <= 40) R_Zoom <= 1;
		else R_Zoom <= 0;
	end
	else if (Zoom == 1 && R_x_max > 280 && R_x_min < 360) begin	//This will check after beacon
		R_found <= 8'h01;														//spotted and zoomed in
		R_Zoom <= 1;
	end
	else begin																	//Beacon not spotted
		R_found <= 8'h00;
		R_Zoom <= 0;
	end
	
	//Yellow Detection
	if (Zoom == 0 && average_Y >= CROSSHAIR_L && average_Y <= CROSSHAIR_R && 
		(right_Y >= right_R && top_Y <= top_R) && Y_x_max != 0) begin 
		Y_found <= 8'h01;
		if (Y_y_max - Y_y_min <= 40) Y_Zoom <= 1;
		else Y_Zoom <= 0;
	end
	else if (Zoom == 1 && Y_x_max > 280 && Y_x_min < 360) begin	
		Y_found <= 8'h01;																				
		Y_Zoom <= 1;
	end
	else begin																						
		Y_found <= 8'h00;
		Y_Zoom <= 0;
	end
	
	
	//Blue Detection
	if (Zoom == 0 && average_B >= CROSSHAIR_L && average_B <= CROSSHAIR_R && B_x_max != 0) begin 
		B_found <= 8'h01;
		if (B_y_max - B_y_min <= 40) B_Zoom <= 1;
		else B_Zoom <= 0;
	end
	else if (Zoom == 1 && B_x_max > 280 && B_x_min < 360) begin	
		B_found <= 8'h01;																				
		B_Zoom <= 1;
	end
	else begin																						
		B_found <= 8'h00;
		B_Zoom <= 0;
	end
	
	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	
	//(right_Y < right_R && top_Y > top_R))
	//(Y_x_max < R_x_max && Y_y_min > R_y_min))	

	if (sop & in_valid) begin	//Reset bounds on start of packet for all colours
		R_x_min <= IMAGE_W-11'h1;
		Y_x_min <= IMAGE_W-11'h1;
		B_x_min <= IMAGE_W-11'h1;
		
		R_x_max <= 0;
		Y_x_max <= 0;
		B_x_max <= 0;
		
		R_y_min <= IMAGE_H-11'h1;
		Y_y_min <= IMAGE_H-11'h1;
		B_y_min <= IMAGE_H-11'h1;		

		R_y_max <= 0;
		Y_y_max <= 0;
		B_y_max <= 0;
		
	end
end

//Process bounding box at the end of the frame.
reg [1:0] msg_state; 
reg [10:0] left_R, right_R, top_R, bottom_R; 
reg [10:0] left_Y, right_Y, top_Y, bottom_Y; 
reg [10:0] left_B, right_B, top_B, bottom_B; 
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
	
		//Latch edges for display overlay on next frame
		left_R <= R_x_min;
		right_R <= R_x_max;
		top_R <= R_y_min;
		bottom_R <= R_y_max;

		left_Y <= Y_x_min;
		right_Y <= Y_x_max;
		top_Y <= Y_y_min;
		bottom_Y <= Y_y_max;
		
		left_B <= B_x_min;
		right_B <= B_x_max;
		top_B <= B_y_min;
		bottom_B <= B_y_max;
		
		Zoom <= (R_Zoom || Y_Zoom || B_Zoom);
		
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 2'b01;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 2'b00) msg_state <= msg_state + 2'b01;

end
	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

`define RED_BOX_MSG_ID "RBB"

always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		2'b00: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
		2'b01: begin
			msg_buf_in = {16'b0, R_BB_Height}; // Red BB height in pixels
			msg_buf_wr = 1'b1;
		end
		2'b10: begin
			msg_buf_in = {Zoom, R_found, Y_found, B_found};	//Signals for Red, Yellow or Blue detection and Zoom
			msg_buf_wr = 1'b1;
		end
		2'b11: begin
			msg_buf_in = {Y_BB_Height, B_BB_Height}; //Yellow and Blue BB height in pixels
			msg_buf_wr = 1'b1;
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    				1
`define READ_ID    				2
`define REG_BBCOL					3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
	end
	else begin
		if(s_chipselect & s_write) begin	//If recieving a message from Eclipse
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin	//If writing to eclipse
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};	//
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						


endmodule


