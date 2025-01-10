/**
 * \file struct vector.h
 * \author František Pártl (fpartl@ntis.zcu.cz)
 * \brief Hlavičkový soubor knihovny dynamicky realokovaného pole.
 * \date 2021-11-25
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stddef.h>

/** \brief Násobitel zvětšení pole při realokaci. */
#define VECTOR_SIZE_MULT 2

/** \brief Počáteční velikost vektoru. */
#define VECTOR_INIT_SIZE 4

/**
 * \brief Definice ukazatele na obecnou funkci, která umí uvolnit prvek uložený ve vektoru.
 *        Využívá se ve funkci `vector_deinit`.
 */
typedef void (*vec_it_dealloc_type)(void *item);

/**
 * \brief Struktura `vector`
 * Přestavuje obecné dynamicky alokované pole, do které lze přidávat prvky, manipulovat s nimi a
 * odstraňovat je (včetně jejich případného uvolnění).
 */
struct vector {
    size_t count;                       /**< Počet prvků vektoru. */
    size_t capacity;                    /**< Velikost vektoru. */
    size_t item_size;                   /**< Velikost jednoho prvku vektoru. */
    void *data;                         /**< Data obsažené ve vektoru. */
    vec_it_dealloc_type deallocator;    /**< Ukazatel na funkci, určenou k uvolnění každého z prvků vektoru. */
};

#define UNINITIALIZED_VECTOR {0, 0, 0, NULL, NULL}

/**
 * \brief Dynamicky alokuje prázdný vektor s počáteční kapacitou `VECTOR_INIT_SIZE`.
 * \param item_size Velikost jednoho prvku vektoru.
 * \return struct vector* Ukazatel na alokovaný vektor.
 */
struct vector *vector_allocate(const size_t item_size, const vec_it_dealloc_type deallocator);

/**
 * \brief Funkce provede inicializaci instance struktury `vector`, která je dána ukazatelem `v`. Datový segment
 *        bufferu bude prealokován pro `VECTOR_INIT_SIZE` prvků o velikosti `item_size`.
 *
 * \param v Ukazatel na inicializovanou instanci struktury `vector`.
 * \param item_size Velikost jednoho prvku vektoru.
 * \param deallocator Ukazatel na dealokátor alokovaných prvků vektoru (taková emulace destruktoru).
 * \return int 1, pokud inicializace proběhla v pořádku, jinak 0.
 */
int vector_init(struct vector *v, const size_t item_size, const vec_it_dealloc_type deallocator);

/**
 * \brief Funkce uvolní prostředky držené vektorem, na který ukazuje pointer `v`. Tato funkce se nám bude hodit, pokud
 *        samotný vektor (instanci struktury) alokujeme automaticky na zásobníku libovolné funkce. Na zásobníku tedy nebudeme
 *        alokovat 8B pointer, který bychom při použití museli vždy dereferencovat, ale místo toho na zásobníku funkce alokujeme
 *        rovnou celou instanci struktury `vector`, která není nikterak velká (dokud se budeme při práce se zásobníkem
 *        držet pod hranicí 100 kB, tak nemusíme bát přetečení). Při uvolňování automaticky alokované instance struktury `vector`
 *        tedy budeme chtít pouze uvolnit jí držená data (buffer a případně vynulovat paměť), ale samotnou instanci struktury
 *        uvolňovat nebudeme, to se stane automaticky při ukončení funkce. V případě platnosti ukazatele `v->deallocator` je
 *        funkce, na kterou ukazatel odkazuje, zavolána nad všemi prvky vektoru (emulace destruktoru).
 * \param v Ukazatel na vektor.
 */
void vector_deinit(struct vector *v);

/**
 * \brief Uvolní data držená vektorem na adrese `*v` z operační paměti počítače pomocí funkce `vector_deinit` a následně uvolní
 *        i dynamicky alokovanou instanci struktury `vector` a zneplatní ukazatel na ní.
 * \param v Ukazatel na ukazatel na vektor.
 */
void vector_deallocate(struct vector **v);

/**
 * \brief Vrátí kapacitu vektoru (počet prvků, které do vektoru lze uložit bez realokace).
 * \param v Ukazatel na vektor.
 * \return size_t Kapacita vektoru, daného ukazatelem v.
 */
size_t vector_capacity(const struct vector *v);

/**
 * \brief Vrátí počet prvků vektoru.
 * \param v Ukazatel na vektor.
 * \return int Počet zapsaných prvků ve vektoru.
 */
size_t vector_count(const struct vector *v);

/**
 * \brief Říká, zda je zadaný vektor prázdný nebo obsahuje nějaké prvky.
 * \param v Ukazatel na vektor.
 * \return int 1, pokud je vektor prázdný, jinak 0.
 */
int vector_isempty(const struct vector *v);

/**
 * \brief Realokuje zadaný vektor tak, aby jeho kapacita byla `capacity`. Pro jednoduchost tato implementace nepovoluje
 *        zmenšení vektoru -- zkuste si naprogramovat funkci `vector_shrink` samostatně.
 * \param v Realokovaný vektor.
 * \param capacity Nová kapacita vektoru.
 * \return int 1, pokud se realokace povedla, jinak 0.
 */
int vector_realloc(struct vector *v, const size_t capacity);

/**
 * \brief Vrací ukazatel na at-tý prvek vektoru `v`. V případě neexistence prvku vratí `NULL`.
 * \param v Ukazatel na vektor.
 * \param at Index prvku ve vektoru.
 * \return void* Ukazatel na at-tý prvek vektoru nebo NULL při neexistenci prvku.
 */
void *vector_at(const struct vector *v, const size_t at);

/**
 * \brief Přidá prvek do vektoru. V případě chyby není vektor daný ukazatel v změněn.
 * \param v Ukazatel na vektor, kam se prvek přidává.
 * \param item Ukazatel na přidávaný prvek.
 * \return int 1, pokud se přidání prvku povedlo, jinak 0.
 */
int vector_push_back(struct vector *v, const void *item);

/**
 * \brief Funkce zadanému vektoru odebere vlastnictví udržovaného ukazatele na dynamicky
 *        alokovaný buffer. Tzn. že programátor přebírá plnou zodpovědnost na vráceným ukazatel
 *        na dynamicky alokovaná data. Vektor bude nakonec znovu inicializován pomocí funkce `vector_init`.
 * \param v Ukazatel na vektor, nad jehož daty přebíráme kontrolu.
 * \return void* Ukazatel na data získaná z vektoru.
 */
void *vector_give_up(struct vector *v);

void vector_remove(struct vector *v, const size_t at);

#endif
