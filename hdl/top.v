`define VIVADO

`timescale 1ns / 1ps

module top(
    input clk,
    input rst
    );
    
    (*dont_touch = "true"*) wire [127:0] key = 128'hda3da63adf803d95c41a0230dbb8f3fd;
    (*dont_touch = "true"*) wire [127:0] mask = 128'h7dbdb314b324f4667ff8e52ab24a9eee;
           
    (*dont_touch = "true"*) wire [127:0] hash;
    

    hash64 h64_1
    (
        .clk(clk),
    
        .key(key[63:0]),
        .mask(mask[63:0]),
    
        .hash(hash[63:0])
    );  

    hash64 h64_2
    (
        .clk(clk),
    
        .key(key[127:64]),
        .mask(mask[127:64]),
    
        .hash(hash[127:64])
    );  

    (*dont_touch = "true"*) reg [5:0] ctr_r;
    
    always @(posedge clk) begin
        if (rst) begin
            ctr_r <= 6'b0;
        end
        else begin
            ctr_r <= ctr_r + 1;
        end
    end

    `ifdef VIVADO
    (*dont_touch = "true"*) wire[159:0] bmem_dout;
    blk_mem_chain bmem
    (
        .addra(ctr_r),
        .clk(clk),
        .douta(bmem_dout),
        .ena(1'b1)
    );
    
    `else
    
    
    `endif

    (*dont_touch = "true"*) wire [255:0] wi = 256'h74d9b4c90913e55f30e95152ecbdfe1110f35cfacc712cad2cfd390b92d162bc;  
    (*dont_touch = "true"*) wire [31:0] f_in = 32'hc4819652;   
    (*dont_touch = "true"*) wire [224:0] f_int;
                               
    (*dont_touch = "true"*) wire [31:0] result;   
                       
    genvar i;
    generate    
        for (i = 0 ; i < 8 ; i = i+1) begin : generate_for
            chain chaining
            (
                .clk(clk),
                
                .xi_i(bmem_dout[63:0]),
                .xj_i(bmem_dout[127:64]),
                .yi_i(bmem_dout[143:128]),
                .yj_i(bmem_dout[160:144]),
                .wi_i(wi[i*32 +: 32]),
                .f_i(i == 0 ? f_in : f_int[(i-1)*32 +: 32]),
                
                .result(i == 8 ? result : f_int[i*32 +: 32])
            );
        end
    endgenerate
  
endmodule
