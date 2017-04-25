# General guidelines

* `pcs->pitch_content`:
We set this pointer to `NULL` after freeing it. That's how we know if it has already been freed or non-allocated. It should also be good to set `pcs->nelem` to 0 and mark as non consistent.

* Global constants:
Para las constantes globales, definidas como macros, deberíamos usar valores en el rango `0x8000` - `0x800F` en hexadecimal, así no tenemos problemas si pasamos de signed a unsigned, etc.