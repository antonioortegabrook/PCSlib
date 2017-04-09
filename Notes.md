# General guidelines

* `pcs->pitch_content`:
We set this pointer to `NULL` after freeing it. That's how we know if it has already been freed or non-allocated. It should also be good to set `pcs->nelem` to 0 and mark as non consistent.