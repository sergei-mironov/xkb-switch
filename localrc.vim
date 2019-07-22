" Set the identation size
function! Ident(ident_spaces)
  let &expandtab=1
  let &shiftwidth=a:ident_spaces
  let &tabstop=a:ident_spaces
  let &cinoptions="'g0,(".a:ident_spaces
  let &softtabstop=a:ident_spaces
endfunction

call Ident(2)
