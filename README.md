# Bachelor_thesis
Bakalářská práce - bezeztrátová komprese obrazu s využitím vlnkové transformace.
Práce je pod licencí GNU Lesser General Public License 2.1 uloženou v souboru LICENSE, výjimku tvoří soubor thirdparty, který obsahuje složky s vlastní licencí.

Program je překládán pomocí CMake, Make.
Pro kompilaci knihovny je nutná podpora vícevláknového programování (standardní knihovna C++).
Pro kompilaci programu je navíc nutné mít Boost C++, CImg knihovna (součást zdroje), libpng.

## Program
Použití programu je stručně shrnuto v nápovědě programu (-h, --help).
Kvůli bezeztrátovosti program přijme a vytváří soubory formátu PNG.



## Knihovna
Knihovnu lze ovládat pomocí tříd WTencoder a WTdecoder (potažmo WTcommon).
Jako příklad použití je připraven ukázkový program.

### Kódování
Pro zakódování dat je nutné zvolit metodu zpracování.
Toto se provádí třídou WTcommon, které se nastaví třídní proměnné.

barevny_prostor, vlnka, dekompozice, dekompozice_max (počet úrovní), dekompozice_min_rozmer (pokračování další WT při splnění tohoto rozměru), dekompozice_side_level (rozklad vedlejších pásem viz práce), prediktor_pred (prediktor na originální data), prediktor_pasmo (výběr zvláštního prediktoru pro dané pásmo), koder a vyjimka_koder (vyběr entropického kodéru)

Další krok je vkládání pásem, pomocí metody WTencoder::vlozit_pasmo.
Poslední fáze je zavolání WTencoder::start( std::string cesta_vystupu ).

### Dekódování
Dekódování probíha jednodušeji, spustí se WTdecoder::start( std::string cesta ), to je vše.
Každý komprimovaný soubor s sebou nese informaci o způsobu komprimace, takže není nutné vyplňovat WTcommon.
WTcommon nicméně může sloužit pro zpětné zjištění způsobu komprese předaného souboru.






