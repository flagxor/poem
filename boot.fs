: _last_flag   _dict_head @ cell+ cell+ cell+ ;
: immediate   _last_flag @ 1 or _last_flag ! ;

: '   _parse_word find drop ;
: literal    __lit , , ;
: [']   ' literal ; immediate

: if   ['] 0branch , here 0 , ; immediate
: ahead   ['] branch , here 0 , ; immediate
: then   here over - swap ! ; immediate
: begin   here ; immediate
: until   ['] 0branch , here - , ; immediate
: again   ['] branch , here - , ; immediate

: terminate   >r 0 0 0 0 0 r> SYS_exit syscall ;
: emit   >r 0 0 0 1 rp@ 1 SYS_write syscall rdrop drop ;
: type   swap >r >r 0 0 0 r> r> 1 SYS_write syscall rdrop drop ;

: postpone
    _parse_word find 1 =
    if , ahead [ swap ] then literal ['] , , then ; immediate

: else   postpone ahead swap postpone then ; immediate
: while   postpone if swap ; immediate
: repeat   postpone again postpone then ; immediate

: do   postpone swap postpone >r postpone >r postpone begin
       postpone r> postpone r@ postpone swap postpone >r
       postpone r@ postpone <> postpone while ; immediate
: +loop   postpone r> postpone + postpone >r
          postpone repeat postpone rdrop postpone rdrop ; immediate
: loop   postpone 1 postpone +loop ; immediate
: for   postpone 0 postpone do ; immediate
: next   postpone loop ; immediate
: i   postpone r@ ; immediate
: j   postpone r> postpone r> postpone r@ postpone -rot
      postpone >r postpone >r ; immediate

: constant   _docon 0 _new_word , ;
: variable   _dovar 0 _new_word 0 , ;
variable _user_var_count   2 2* 2* _user_var_count !
: _user_slot    _user_var_count @ 1 _user_var_count +! ;
: user   _douvar 0 _new_word _user_slot , ;

: code   _docode ;
: end-code   ;

user _catcher
: catch   sp@ >r _catcher @ >r rp@ _catcher ! execute
          r> _catcher ! r> drop 0 ;
: throw   dup 0= if drop exit then
          _catcher @ rp! r> _catcher ! r> swap >r sp! drop r> ;

: rot   >r swap r> swap ;
: -rot   swap >r swap r> ;

2 2* 2* 2+ constant 10
10 10 + 10 + 10 + 2 2* 2* + constant '0'
: +digit   '0' - swap 10 * + ;
: range   over + swap ;
: _#   _parse_word 0 -rot range do i c@ +digit loop ;
: #   _# state @ if literal then ; immediate

# 97 constant 'a'
# 122 constant 'z'
# 65 constant 'A'
: upper   dup 'a' >= over 'z' <= and if 'a' - 'A' + then ;

'0' # 9 + constant '9'
: hex>d   upper dup '9' > if 'A' - 10 + else '0' - then ;
: +hdigit   hex>d swap # 16 * + ;
: _$   _parse_word 0 -rot range do i c@ +hdigit loop ;
: $   _$ state @ if literal then ; immediate

: xt>name   # 3 cells - dup @ swap 1 cells + @ ;
: xt>rest   1 cells + ;
: xt.   xt>name type ;
: see   ' xt>rest begin dup @ ['] _semiexit <> while dup @ xt. cell+ repeat ;

: nl   10 emit ;
: bye   0 terminate ;

: test1 $ 14 for i 'A' + emit loop nl ;
test1

bye
