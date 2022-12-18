def lower_case_to_value (ch: i8): i32 = 
    ch - 'a' |> i32.i8
    
def to_flag (num:i32) =
    1 << num
    
def index_of 'a [n] (p: a -> bool) (as: [n]a) : i64 =
  (loop (_res, i) = (-1, 0) 
   while i < n do
     if p as[i]
     then (i, n)
     else (-1, i+1)
   ).0 

entry is_marker (str: []i8): bool =
    let amount_unique = str |> map lower_case_to_value |> map to_flag |> reduce (|) 0 |> i32.popc
    in 
        amount_unique == (length str |> i32.i64)

entry solve (len: i64) (str: []i8) =
    let starting_indices = iota ((length str) - len)
    in
        starting_indices |> index_of (\x -> is_marker str[x:(x+len)]) |> (+) len