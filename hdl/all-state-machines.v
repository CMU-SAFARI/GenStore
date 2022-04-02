module sm
(
  input clk,
  input rst,

  // SM1
  input seeds_g_n,
  input seeds_s_m,
  input found,

  output channel_read_sm1,
  output send_reads_sm1,
  output hash_sm1,
  output chaining_start_sm1,

  // SM 2
  input present_sm2,
  input maybe_present_sm2,

  output channel_read_sm2,
  output send_reads_sm2,
  output channel_write_sm2,
  output bitmap_write_sm2,
  output hash_sm2,

  // SM 3 - GenStore_HLR
  input match_sm3,

  output channel_read_sm3,
  output hash_sm3,
  output collect_seed_pos_sm3,
  output send_reads_sm3,
  
  // SM 4 - Genstore_MP
  input maybe_present_sm4,

  output send_reads_sm4,
  output channel_read_sm4,
  output hash_sm4

);

  assign channel_read_sm1 = sm1_r == 3'b001;
  assign send_reads_sm1 = sm1_r == 3'b011;
  assign hash_sm1 = sm1_r == 3'b010;
  assign chaining_start_sm1 = sm1_r == 3'b101;

  reg [2:0] sm1_r, sm1_ns;

  always @* begin
    sm1_ns = sm1_r;
    case (sm1_r)
      3'b000:
        sm1_ns = 3'b001;
      3'b001:
        sm1_ns = 3'b010;
      3'b010: begin
        if (seeds_g_n) 
          sm1_ns = 3'b011;
        else if (seeds_s_m)
          sm1_ns = 3'b100;
        else
          sm1_ns = 3'b101;
      end
      3'b011:
        sm1_ns = 3'b000;
      3'b100:
        sm1_ns = 3'b000;
      3'b101: begin
        if (found)
          sm1_ns = 3'b011;
        else
          sm1_ns = 3'b100;
      end

    endcase
  end

  always @(posedge clk) begin
    if (rst) begin
      sm1_r <= 0;
    end
    else begin
      sm1_r <= sm1_ns;
    end
  end

  assign channel_read_sm2 = (sm1_r == 3'b001) || (sm1_r == 3'b101);
  assign send_reads_sm2 = sm1_r == 3'b011;
  assign hash_sm2 = sm2_r == 3'b010;
  assign channel_write_sm2 = sm2_r == 3'b110;
  assign bitmap_write_sm2 = sm2_r == 3'b100;

  reg [2:0] sm2_r, sm2_ns;

  always @* begin
    sm2_ns = sm2_r;
    case (sm2_r)
      3'b000:
        sm2_ns = 3'b001;
      3'b001:
        sm2_ns = 3'b010;
      3'b010: begin
        if (present_sm2) 
          sm2_ns = 3'b011;
        else if (maybe_present_sm2)
          sm2_ns = 3'b100;
      end
      3'b011:
        sm2_ns = 3'b000;
      3'b100:
        sm2_ns = 3'b101;
      3'b101:
        sm2_ns = 3'b110;
      3'b110:
        sm2_ns = 3'b000;
    endcase
  end

  always @(posedge clk) begin
    if (rst) begin
      sm2_r <= 0;
    end
    else begin
      sm2_r <= sm2_ns;
    end
  end

  assign channel_read_sm3 = sm3_r == 3'b001;
  assign hash_sm3 = sm3_r == 3'b010;
  assign collect_seed_pos_sm3 = sm3_r == 3'b011;
  assign send_reads_sm3 = sm3_r == 3'b100;

  reg [2:0] sm3_r, sm3_ns;

  always @* begin
    sm3_ns = sm3_r;
    case (sm3_r)
      3'b000:
        sm3_ns = 3'b001;
      3'b001:
        sm3_ns = 3'b010;
      3'b010: begin
        if (match_sm3) 
          sm3_ns = 3'b000;
        else
          sm3_ns = 3'b011;
      end
      3'b011:
        sm3_ns = 3'b100;
      3'b100:
        sm3_ns = 3'b000;
    endcase
  end

  always @(posedge clk) begin
    if (rst) begin
      sm3_r <= 0;
    end
    else begin
      sm3_r <= sm3_ns;
    end
  end


  assign send_reads_sm4 = sm4_r == 2'b11;
  assign channel_read_sm4 = sm4_r == 2'b01;
  assign hash_sm4 = sm4_r == 2'b10;

  reg [1:0] sm4_r, sm4_ns;

  always @* begin
    sm4_ns = sm4_r;
    case (sm4_r)
      2'b00:
        sm4_ns = 2'b01;
      2'b01:
        sm4_ns = 2'b10;
      2'b10: begin
        if (maybe_present_sm4) 
          sm4_ns = 2'b11;
        else
          sm4_ns = 2'b00;
      end
      2'b11:
        sm4_ns = 2'b00;
    endcase
  end

  always @(posedge clk) begin
    if (rst) begin
      sm4_r <= 0;
    end
    else begin
      sm4_r <= sm4_ns;
    end
  end


endmodule