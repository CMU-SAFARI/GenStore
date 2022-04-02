`define MULTICYCLE

module hash64
  (
    input clk,

    input [63:0] key,
    input [63:0] mask,

    output [63:0] hash
  );

  // ============== minimap2's source for hash64 ========================
  // static inline uint64_t hash64(uint64_t key, uint64_t mask)
  // {
  // 	key = (~key + (key << 21)) & mask; // key = (key << 21) - key - 1;
  // 	key = key ^ key >> 24;
  // 	key = ((key + (key << 3)) + (key << 8)) & mask; // key * 265
  // 	key = key ^ key >> 14;
  // 	key = ((key + (key << 2)) + (key << 4)) & mask; // key * 21
  // 	key = key ^ key >> 28;
  // 	key = (key + (key << 31)) & mask;
  // 	return key;
  // }

  `ifdef PIPELINED
  // ==================== CHAD PIPELINED HASH64 ===========================

  // Each line in the C code corresponds
  // to a pipeline stage in the pipelined version
  // e.g.,
  // Stage 1 --> key = (~key + (key << 21)) & mask;
  // Stage 2 --> key = key ^ key >> 24;

  reg [63:0] s1_key, s1_mask;
  reg [63:0] s2_key, s2_mask;
  reg [63:0] s3_key, s3_mask;
  reg [63:0] s4_key, s4_mask;
  reg [63:0] s5_key, s5_mask;
  reg [63:0] s6_key, s6_mask;
  reg [63:0] s7_key;

  assign hash = s7_key;

  always @(posedge clk) begin
    s1_key <= ((~key) + {key[42:0],{21{1'b0}}}) & mask;
    s1_mask <= mask;
    
    s2_key <= s1_key ^ {{24{1'b0}},s1_key[63:24]};
    s2_mask <= s1_mask;
    
    s3_key <= ((s2_key + {s2_key[60:0], {3{1'b0}}}) + {s2_key[55:0],{8{1'b0}}}) & s2_mask;
    s3_mask <= s2_mask;

    s4_key <= s3_key ^ {{14{1'b0}},s3_key[63:14]};
    s4_mask <= s3_mask;

    s5_key <= ((s4_key + {s4_key[61:0], {2{1'b0}}}) + {s4_key[59:0],{4{1'b0}}}) & s4_mask;
    s5_mask <= s4_mask;

    s6_key <= s5_key ^ {{28{1'b0}},s5_key[63:28]};
    s6_mask <= s5_mask;

    s7_key <= (s6_key + {s6_key[32:0],{32{1'b0}}}) & s6_mask;
  end

  `elsif MULTICYCLE
  reg [63:0] round_key_r, round_mask_r;
  reg [63:0] round_key_ns, round_mask_ns;

  reg [2:0] round_r, round_ns;

  assign hash = round_key_r;

  always @* begin
    round_ns = round_r;
    round_key_ns = round_key_r;
    round_mask_ns = round_mask_r;
    case (round_r)
      3'b000: begin
        round_key_ns = key;
        round_mask_ns = mask;
        round_ns = round_r + 1;
      end
      3'b001: begin
        round_key_ns = ((~round_key_r) + {round_key_r[42:0],{21{1'b0}}}) & round_mask_r;
        round_ns = round_r + 1;
      end
      3'b010: begin
        round_key_ns = round_key_r ^ {{24{1'b0}},round_key_r[63:24]};
        round_ns = round_r + 1;
      end
      3'b011: begin
        round_key_ns = ((round_key_r + {round_key_r[60:0], {3{1'b0}}}) + {round_key_r[55:0],{8{1'b0}}}) & round_mask_r;
        round_ns = round_r + 1;
      end
      3'b100: begin
        round_key_ns = round_key_r ^ {{14{1'b0}},round_key_r[63:14]};
        round_ns = round_r + 1;
      end
      3'b101: begin
        round_key_ns = ((round_key_r + {round_key_r[61:0], {2{1'b0}}}) + {round_key_r[59:0],{4{1'b0}}}) & round_mask_r;
        round_ns = round_r + 1;
      end
      3'b110: begin
        round_key_ns = round_key_r ^ {{28{1'b0}},round_key_r[63:28]};
        round_ns = round_r + 1;
      end
      3'b111: begin
        round_key_ns = (round_key_r + {round_key_r[32:0],{32{1'b0}}}) & round_mask_r;
        round_ns = 0;
      end
    endcase
  end

  always @(posedge clk) begin
    round_r <= round_ns;
    round_key_r <= round_key_ns;
    round_mask_r <= round_mask_ns;
  end
 
  `else

  // ==================== VIRGIN SINGLE-CYCLE HASH64 =======================
  // TODO: should we register the output?

  reg [63:0] key_i;
  reg [63:0] mask_i;

  reg [63:0] s1_key;
  reg [63:0] s2_key;
  reg [63:0] s3_key;
  reg [63:0] s4_key;
  reg [63:0] s5_key;
  reg [63:0] s6_key;
  reg [63:0] s7_key;
  reg [63:0] key_r;

  assign hash = key_r;

  always @* begin
    s1_key = ((~key_i) + {key_i[42:0],{21{1'b0}}}) & mask_i;
    s2_key = s1_key ^ {{24{1'b0}},s1_key[63:24]};
    s3_key = ((s2_key + {s2_key[60:0], {3{1'b0}}}) + {s2_key[55:0],{8{1'b0}}}) & mask_i;
    s4_key = s3_key ^ {{14{1'b0}},s3_key[63:14]};
    s5_key = ((s4_key + {s4_key[61:0], {2{1'b0}}}) + {s4_key[59:0],{4{1'b0}}}) & mask_i;
    s6_key = s5_key ^ {{28{1'b0}},s5_key[63:28]};
    s7_key = (s6_key + {s6_key[32:0],{32{1'b0}}}) & mask_i;
  end

  always @(posedge clk) begin
    key_i <= key;
    mask_i <= mask;
    key_r <= s7_key;
  end

  `endif

endmodule
