
module chain
(
  input clk,

  input signed [31:0] xi_i,
  input signed [31:0] xj_i,
  input signed [31:0] yi_i,
  input signed [31:0] yj_i,
  input signed [31:0] wi_i,
  input signed [31:0] f_i,

  output signed [31:0] result
  );

  localparam CONSTANT_VALUE = 27;

  reg [31:0] xi;
  reg [31:0] xj;
  reg [31:0] yi;
  reg [31:0] yj;
  reg [31:0] wi;
  reg [31:0] f;

  reg signed [31:0] y_sub_y, x_sub_x;
  reg signed [31:0] l;
  reg signed [31:0] sel_xy;
  reg signed [31:0] alpha;
  reg signed [31:0] abs_l;
  reg signed [31:0] beta;
  reg signed [31:0] result_r;
  reg signed [31:0] result_reg_r;

  assign result = result_reg_r;

  integer i; // for loop

  reg [31:0] log2_out;

  always @* begin

    y_sub_y = yi - yj;
    x_sub_x = xi - xj;

    if (y_sub_y > x_sub_x)
      sel_xy = x_sub_x;
    else
      sel_xy = y_sub_y;

    if (wi > sel_xy)
      alpha = sel_xy;
    else
      alpha = wi;

    l = y_sub_y - x_sub_x;

    abs_l = {{1'b0},l[30:0]};

    // TODO: log2 in our case is the position of
    // the highest bit?
    for (i = 0 ; i < 32 ; i = i+1) begin : log2for
      if (abs_l[i])
        log2_out = i + 1;
    end

    // We multiply abs_l by 0.125, i.e. shift right by 3
    beta = (log2_out >> 1) + (abs_l >> 3);

    result_r = (alpha + f - beta);

  end

  always @(posedge clk) begin
    result_reg_r <= result_r;
    xi <= xi_i;
    xj <= xj_i;
    yi <= yi_i;
    yj <= yj_i;
    wi <= wi_i;
    f <= f_i;
  end

endmodule