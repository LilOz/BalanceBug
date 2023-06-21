
module RGBtoHSV(
			Rin,
			Gin,
			Bin,
			Colour
		);

		input	[7:0]	Rin;				
		input	[7:0]	Gin;
		input	[7:0]	Bin;

		output reg [2:0] Colour; //Colour coding detected colour as "000 = Red, 001 = Yellow, 010 = Blue, 011 = White, 100 = Grey"
		
		//Make R, G and B 11 bit inputs
		wire [15:0] R;
		wire [15:0] G;
		wire [15:0] B;
		
		assign R = {8'b0, Rin};
		assign G = {8'b0, Gin};
		assign B = {8'b0, Bin};
		
		//State other variables
		reg [15:0] S, V;
		reg [15:0] H;
		reg [15:0] max, min;

		wire [15:0] dif;
		
		
		//Finding the maximum and minimum
		always @ (*) begin
			if 		(R > G && R > B) 	max <= R;
			else if 	(G > B && G > R) 	max <= G;
			else 								max <= B;   

			if 		(R < G && R < B) 	min <= R;
			else if 	(G < B && G < R) 	min <= G;
			else 								min <= B;   
		end
		
		//Finding the difference between max and min
		assign dif = max - min;
		
		//Finding and assigning the H, S and V
		always @ (*) begin
			//S
			if (max == 0) 	S <= 0;
			else 				S <= (dif*100)/max;
			
			//V
			V <= (max*100)/255;
			
			//H
			if 		(dif == 0) H <= 0;
			else if 	(R > G && R > B) begin	//If Red max
				if		(G > B) 						H <= ((G - B) * 60)/dif;					
				else 									H <= ((B - G) * 60)/dif;
			end
			
			else if 	(G > B && G > R) begin	//If Green max
				if		(B > R) 						H <= (((B - R) * 60)/dif) + 120;					
				else 									H <= 120 - (((R - B) * 60)/dif);
			end
			
			else begin								//If Blue max then check if R is < or > than G
				if		(R > G) 					H <= (((R - G) * 60)/dif) + 240;					
				else 								H <= 240 - (((G - R) * 60)/dif);
			end
			
		end
		
		always @ (*) begin
			if (V >= 100) begin
				if (S <= 10) Colour <= 3'b011;	//White
				else if (S >= 70) begin
					if (H <= 240 && H >= 180) 			Colour <= 3'b010;	//Blue
					else if (H <= 70 && H >= 50) 		Colour <= 3'b001;	//Yellow
					else if (H <= 10) 					Colour <= 3'b000;	//Red
					else 										Colour <= 3'b100;	//Grey					
				end 
				else Colour <= 3'b100;	//Grey
			end
			
			else Colour <= 3'b100;	//Grey
		
		end
		
endmodule


/*
module RGBtoHSV(
			Rin,
			Gin,
			Bin,
			Colour
		);

		input	[7:0]	Rin;				
		input	[7:0]	Gin;
		input	[7:0]	Bin;

		output reg [2:0] Colour; //Colour coding detected colour as "000 = Red, 001 = Yellow, 010 = Blue, 011 = White, 100 = Grey"
		
		//Make R, G and B 11 bit inputs
		wire signed [15:0] R;
		wire signed [15:0] G;
		wire signed [15:0] B;
		
		assign R = {8'b0, Rin};
		assign G = {8'b0, Gin};
		assign B = {8'b0, Bin};
		
		//State other variables
		reg signed [15:0] S, V;
		reg signed [15:0] H;
		reg signed [15:0] max, min;
		reg signed [15:0] cal;

		wire signed [15:0] dif;
		
		
		//Finding the maximum and minimum
		always @ (*) begin
			if 		(R > G && R > B) 	max <= R;
			else if 	(G > B && G > R) 	max <= G;
			else 								max <= B;   

			if 		(R < G && R < B) 	min <= R;
			else if 	(G < B && G < R) 	min <= G;
			else 								min <= B;   
		end
		
		//Finding the difference between max and min
		assign dif = max - min;
		
		//Finding and assigning the H, S and V
		always @ (*) begin
			//S
			if (max == 0) 	S <= 0;
			else 				S <= (dif*100)/max;		
			
			//V
			V <= (max*100)/255;
			
			//H
			if 		(dif == 0) 			H <= 0;
			else if 	(R > G && R > B) 	H <= ((G - B) * 60)/dif;
			else if 	(G > B && G > R) 	H <= ((B - R) * 60)/dif + 120;	
			else 								H <= ((R - G) * 60)/dif + 240;
			
		end
		
		always @ (*) begin
			if (V >= 100) begin
				if (S <= 10) Colour <= 3'b011;	//White
				else if (S >= 70) begin
					if (H <= 240 && H >= 180) 			Colour <= 3'b010;	//Blue
					else if (H <= 70 && H >= 50) 		Colour <= 3'b001;	//Yellow
					else if (H <= 10 && H >= -10) 	Colour <= 3'b000;	//Red
					else 										Colour <= 3'b100;	//Grey					
				end 
				else Colour <= 3'b100;	//Grey
			end
			
			else Colour <= 3'b100;	//Grey
		
		end
		
endmodule
*/

		
			
