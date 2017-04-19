kanji_codes_status_t
jis2euc (two_bytes jis, two_bytes * euc)
{
    * euc = jis | 0x8080;
    OK;
}
