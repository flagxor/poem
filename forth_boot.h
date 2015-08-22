#ifndef _forth_boot_h_
#define _forth_boot_h_

char boot[] =
" : _last_flag   _dict_head @ cell+ cell+ cell+ ; "
" : immediate   _last_flag @ 1 or _last_flag ! ; "

" : '   _parse_word find drop ; "
" : literal    [ ' _lit , ' _lit , ] , , ; "
" : literal    __lit , , ; "
" : [']   ' literal ; immediate "
" : postpone   ' , ; immediate "

" : if   ['] 0branch , here ; immediate "
" : ahead   ['] branch , here ; immediate "
" : then   here over - swap ! ; immediate "
" : begin   here ; immediate "
" : until   ['] 0branch , here - , ; immediate "
" : again   ['] branch , here - , ; immediate "

" : else   postpone ahead swap postpone then ; immediate "
" : while   postpone if swap ; immediate "
" : repeat   postpone ahead postpone then ; immediate "

" : constant   _docon 0 _new_word , ; "
" : variable   _dovar 0 _new_word 0 , ; "
" variable _user_var_count   2 2* 2* _user_var_count ! "
" : _user_slot    _user_var_count @ 1 _user_var_count +! ; "
" : user   _douvar 0 _new_word _user_slot , ; "

" user _catcher "
" : catch   sp@ >r _catcher @ >r rp@ _catcher ! execute "
"           r> _catcher ! r> drop 0 ; "
" : throw   dup 0= if drop exit then "
"           _catcher @ rp! r> _catcher ! r> swap >r sp! drop r> ; "

" user base "

" : terminate   >r 0 0 0 0 0 r> SYS_exit syscall ; "
" : emit   >r 0 0 0 1 rp@ 1 SYS_write syscall rdrop drop ; "
" 2 2* 2* 2+ constant 10 "
" 10 10 + 10 + 10 + 10 + 10 + 2+ 2+ 1+ constant 65 "
" : letter   65 base @ + 1 base +! emit ; "
" : nl   10 emit ; "
" : bye   0 terminate ; "
" : test   10 10 + begin letter 1- dup 0< until drop nl bye ; "
" test "
;

#endif
