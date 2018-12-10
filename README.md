# JNP1_5
Bajtek, po krachu jego cyfrowej waluty, zostaĹ zmuszony wrĂłciÄ do kariery
akademickiej. PoniewaĹź nie jest zadowolony ze swojej nowej pensji, pisze obecnie
wniosek o awans na stanowisko profesora. We wniosku musi podkreĹliÄ âwpĹywowoĹÄâ
swoich publikacji. W tym celu potrzebuje przeprowadziÄ analizÄ cytowaĹ.

StrukturÄ, ktĂłra umoĹźliwia przeprowadzenie takiej analizy, jest graf cytowaĹ.
Graf cytowaĹ jest grafem skierowanym acyklicznym. KaĹźdy wierzchoĹek grafu
odpowiada jednej publikacji. Graf ma jedno ĹşrĂłdĹo, ktĂłre reprezentuje pierwotnÄ
publikacjÄ, ktĂłrej wpĹyw chcemy analizowaÄ. KrawÄdĹş ĹÄczy publikacjÄ cytowanÄ
z publikacjÄ cytujÄcÄ.

NaleĹźy zaimplementowaÄ szablon klasy, ktĂłra reprezentuje taki graf.

template <class Publication> CitationGraph;

Klasa Publication reprezentuje informacje o publikacji. Jej implementacja
zostanie dostarczona w stosownym czasie.

Klasa CitationGraph powinna udostÄpniaÄ nastÄpujÄcy interfejs.

// Tworzy nowy graf. Tworzy takĹźe wÄzeĹ publikacji o identyfikatorze stem_id.
CitationGraph(Publication::id_type const &stem_id);

// Konstruktor przenoszÄcy i przenoszÄcy operator przypisania. Powinny byÄ
// noexcept.
CitationGraph(CitationGraph<P> &&other);
CitationGraph<P>& operator=(CitationGraph<P> &&other);

// Zwraca identyfikator ĹşrĂłdĹa. Metoda ta powinna byÄ noexcept wtedy i tylko
// wtedy, gdy metoda Publication::get_id jest noexcept. Zamiast pytajnika naleĹźy
// wpisaÄ stosowne wyraĹźenie.
Publication::id_type get_root_id() const noexcept(?);

// Zwraca listÄ identyfikatorĂłw publikacji cytujÄcych publikacjÄ o podanym
// identyfikatorze. ZgĹasza wyjÄtek PublicationNotFound, jeĹli dana publikacja
// nie istnieje.
std::vector<Publication::id_type> get_children(Publication::id_type const &id) const;

// Zwraca listÄ identyfikatorĂłw publikacji cytowanych przez publikacjÄ o podanym
// identyfikatorze. ZgĹasza wyjÄtek PublicationNotFound, jeĹli dana publikacja
// nie istnieje.
std::vector<Publication::id_type> get_parents(Publication::id_type const &id) const;

// Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
bool exists(Publication::id_type const &id) const;

// Zwraca referencjÄ do obiektu reprezentujÄcego publikacjÄ o podanym
// identyfikatorze. ZgĹasza wyjÄtek PublicationNotFound, jeĹli ĹźÄdana publikacja
// nie istnieje.
Publication& operator[](Publication::id_type const &id) const;

// Tworzy wÄzeĹ reprezentujÄcy nowÄ publikacjÄ o identyfikatorze id cytujÄcÄ
// publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
// parent_ids. ZgĹasza wyjÄtek PublicationAlreadyCreated, jeĹli publikacja
// o identyfikatorze id juĹź istnieje. ZgĹasza wyjÄtek PublicationNotFound, jeĹli
// ktĂłryĹ z wyspecyfikowanych poprzednikĂłw nie istnieje.
void create(Publication::id_type const &id, Publication::id_type const &parent_id);
void create(Publication::id_type const &id, std::vector<Publication::id_type> const &parent_ids);

// Dodaje nowÄ krawÄdĹş w grafie cytowaĹ. ZgĹasza wyjÄtek PublicationNotFound,
// jeĹli ktĂłraĹ z podanych publikacji nie istnieje.
void add_citation(Publication::id_type const &child_id, Publication::id_type const &parent_id);

// Usuwa publikacjÄ o podanym identyfikatorze. ZgĹasza wyjÄtek
// PublicationNotFound, jeĹli ĹźÄdana publikacja nie istnieje. ZgĹasza wyjÄtek
// TriedToRemoveRoot przy prĂłbie usuniÄcia pierwotnej publikacji.
void remove(Publication::id_type const &id);

ZakĹadamy, Ĺźe:
* klasa Publication ma konstruktor przyjmujÄcy argument typu
  Publication::id_type;
* klasa Publication ma metodÄ Publication::id_type get_id() const, ktĂłra
  dodatkowo moĹźe, ale nie musi byÄ noexcept;
* typ Publication::id_type ma konstruktor bezargumentowy, konstruktor
  kopiujÄcy, konstruktor przenoszÄcy i operatory przypisania;
* wartoĹci typu Publication::id_type tworzÄ porzÄdek liniowy i moĹźna je
  porĂłwnywaÄ za pomocÄ operatorĂłw ==, !=, <=, >=, <, >.

Ponadto:
* wszystkie metody klasy CitationGraph powinny gwarantowaÄ silnÄ odpornoĹÄ
  na wyjÄtki, a tam, gdzie to jest moĹźliwe i poĹźÄdane, powinny byÄ noexcept;
* wszystkie metody powinny byÄ przezroczyste dla wyjÄtkĂłw, czyli powinny
  przepuszczaÄ wszelkie wyjÄtki zgĹaszane przez wywoĹywane przez nie funkcje,
  a obserwowalny stan obiektu nie powinien siÄ zmieniÄ;
* prĂłba uĹźycia konstruktora kopiujÄcego lub kopiujÄcego operatora przypisania
  dla obiektĂłw klasy CitationGraph powinna zakoĹczyÄ siÄ bĹÄdem kompilacji;
* zachowanie obiektu typu CitationGraph po utworzeniu cyklu pozostaje
  niezdefiniowane â nie trzeba wykrywaÄ takiej sytuacji;
* wyjÄtki PublicationAlreadyCreated, PublicationNotFound oraz TriedToRemoveRoot
  powinny byÄ zdefiniowane poza klasÄ CitationGraph i powinny dziedziczyÄ
  z std::exception;
* wyszukiwanie publikacji powinno byÄ szybsze niĹź liniowe.

ZarzÄdzanie pamiÄciÄ powinno byÄ zrealizowane za pomocÄ sprytnych wskaĹşnikĂłw
z biblioteki standardowej.

PrzykĹad uĹźycia znajduje siÄ w pliku citation_graph_example.cc i wypisuje na
standardowe wyjĹcie:

PublicationNotFound
PublicationAlreadyCreated
TriedToRemoveRoot

=== Ustalenia techniczne ===

RozwiÄzanie bÄdzie kompilowane na maszynie students poleceniem

g++ -Wall -Wextra -Og -g -std=c++17

Jako rozwiÄzanie naleĹźy dostarczyÄ plik citation_graph.h, ktĂłry naleĹźy umieĹciÄ
w repozytorium w katalogu

grupaN/zadanie5/ab123456+cd123456

lub

grupaN/zadanie5/ab123456+cd123456+ef123456

gdzie N jest numerem grupy, a ab123456, cd123456, ef123456 sÄ identyfikatorami
czĹonkĂłw zespoĹu umieszczajÄcego to rozwiÄzanie. Katalog z rozwiÄzaniem nie
powinien zawieraÄ innych plikĂłw, ale moĹźe zawieraÄ podkatalog prywatne, gdzie
moĹźna umieszczaÄ rĂłĹźne pliki, np. swoje testy. Pliki umieszczone w tym
podkatalogu nie bÄdÄ oceniane. Nie wolno umieszczaÄ w repozytorium plikĂłw
duĹźych, binarnych, tymczasowych (np. *.o) ani innych zbÄdnych.
