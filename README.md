# OSI2019-Grupa-18 | Sudogu

> Sistem upravljanja događajima

Sudogu je sistem koji korisnicima omogućava pregled i upravljanje događajima u nekom gradu. Za događaje se mogu unositi podaci, mijenjati postojeći, brisati ili pregledati. Osim upravljanja podacima o događajima, sistem podržava i upravljanje podacima o vrstama događaja (kategorije), komentarisanje i igranje kviza o gradu.

Sistem se sastoji iz dva dijela: dijela za administraciju podataka i dijela za prikaz informacija. Dio za administraciju se prvenstveno koristi za unos, promjenu i brisanje podataka, dok se dio za prikaz informacija koristi samo za prikaz informacija o evidentiranim događanjima. Ova dva dijela su realizovani kao zasebne aplikacije.

Cilj sistema je da korisnicima obezbijedi lakše praćenje različitih događanja u gradu.

## Sadržaj

- [OSI2019-Grupa-18 | Sudogu](#osi2019-grupa-18--sudogu)
  - [Sadržaj](#sadr%c5%beaj)
  - [Razvojno okruženje](#razvojno-okru%c5%beenje)
  - [Uputstvo za instaliranje i pokretanje](#uputstvo-za-instaliranje-i-pokretanje)
  - [Uputstvo za korištenje - SudoguAdmin](#uputstvo-za-kori%c5%a1tenje---sudoguadmin)
    - [Prijava na sistem](#prijava-na-sistem)
    - [Registracija](#registracija)
    - [Konfigurisanje parametara grada](#konfigurisanje-parametara-grada)
    - [Odjava sa sistema](#odjava-sa-sistema)
    - [Upravljanje događajima](#upravljanje-doga%c4%91ajima)
      - [Pregled liste događaja](#pregled-liste-doga%c4%91aja)
      - [Dodavanje novog događaja](#dodavanje-novog-doga%c4%91aja)
      - [Sortiranje liste događaja](#sortiranje-liste-doga%c4%91aja)
      - [Pregled detalja postojećeg događaja](#pregled-detalja-postoje%c4%87eg-doga%c4%91aja)
      - [Izmjena podataka o događaju](#izmjena-podataka-o-doga%c4%91aju)
      - [Brisanje događaja](#brisanje-doga%c4%91aja)
    - [Upravljanje kategorijama](#upravljanje-kategorijama)
      - [Pregled kategorija](#pregled-kategorija)
      - [Dodavanje nove kategorije događaja](#dodavanje-nove-kategorije-doga%c4%91aja)
      - [Brisanje kategorije događaja](#brisanje-kategorije-doga%c4%91aja)
  - [Uputstvo za korištenje - SudoguUser](#uputstvo-za-kori%c5%a1tenje---sudoguuser)
    - [Pregled današnjih događaja](#pregled-dana%c5%a1njih-doga%c4%91aja)
    - [Pregled događaja određene kategorije](#pregled-doga%c4%91aja-odre%c4%91ene-kategorije)
    - [Pregled svih budućih događaja](#pregled-svih-budu%c4%87ih-doga%c4%91aja)
    - [Pregled događaja koji su prošli](#pregled-doga%c4%91aja-koji-su-pro%c5%a1li)
    - [Sortiranje događaja](#sortiranje-doga%c4%91aja)
    - [Pregled detalja događaja](#pregled-detalja-doga%c4%91aja)
  - [Release History](#release-history)
  - [Meta](#meta)

## Razvojno okruženje

Aplikacije su razvijene na Visual Studio 2019 (v142) korištenjem Windows SDK verzije 10.

## Uputstvo za instaliranje i pokretanje

Za instaliranje aplikacija potrebno je raspakovati arhivu pod nazivom „Sudogu.zip“. Nakon uspješnog raspakovanja, dobijaju se sljedeći fajlovi:
1.	SudoguAdmin.exe – administratorska aplikacija za upravljanje podacima,
2.	SudoguUser.exe – korisnička aplikacija za pregled i praćenje događaja,
3.	accounts.txt – konfiguracija naloga,
4.	city.txt – konfiguracija parametara grada u kom se događaji prate.

Preporučuje se registracija novog korisnika i konfigurisanje parametara grada u kom se događaji prate (opisano u nastavku) odmah nakon instalacije.
Aplikacije se na nekim sistemima moraju pokrenuti sa administratorskim privilegijama.

## Uputstvo za korištenje - SudoguAdmin

### Prijava na sistem

Po otvaranju aplikacije korisnik ima mogućnost prijave na sistem. Potrebno je unijeti ispravno korisničko ime i lozinku da bi prijava bila moguća.

### Registracija

Registracija se ne obavlja kroz aplikaciju, već se nalog konfiguriše ručno. U instalacionom direktorijumu aplikacija, potrebno je izmijeniti fajl _accounts.txt_. Na kraj tog tekstualnog dokumenta, dodati nove podatke u formatu _korisničko_ime:lozinka_ te sačuvati izmjene. Prilikom prvog sledećeg pokretanja administratorskog dijela sistema, konfiguracija se verifikuje. Ako su podaci uneseni u ispravnom obliku u datoteku, korisniku će biti omogućena prijava. U suprotnom, ispisuje se poruka o neispravnoj konfiguraciji naloga i program se zatvara. Korisnik neće biti u mogućnosti da koristi aplikaciju sve dok konfiguraciju ne ispravi.

### Konfigurisanje parametara grada

Konfigurisanje parametara grada se obavlja takođe ručno. Potrebno je u instalacionom direktorijumu otvoriti fajl _city.txt_. Jedini parametar podržan za sada jeste naziv mjesta u kojem se događaji prate. Ovaj parametar se mijenja tako što se promjeni linja koja počinje sa _name:_ tako da dio nakon _:_ sadrži ime željenog mjesta.

### Odjava sa sistema

Izabrati opciju *Odjava* iz glavnog menija.

### Upravljanje događajima

Upravljanje događajima je omogućeno registrovanim korisnicima.

#### Pregled liste događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija.

#### Dodavanje novog događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija. Po otvaranju liste događaja pritiskom na taster *F9* korisniku se prikazuje novi ekran gdje je potrebno unijeti podatke o novom događaju. Pri izboru kategorije, korisnik mora da izabere neku od već postojećih kategorija. Nakon ispravnog unosa svih podataka o događaju, događaj će se sačuvati i dodati u listu svih događaja, na koju će korisnik biti preusmjeren.

#### Sortiranje liste događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija. Po otvaranju liste događaja i pritiska na taster *F10* pojavljuje se prozor gdje korisnik ima mogućnost izbora kolone po kojoj će se lista sortirati. Po izborom kolone, korisniku se prikazuje sortirana lista događaja.

#### Pregled detalja postojećeg događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija. Po otvaranju liste događaja i pritiska na taster *RETURN* (ili *Enter*) korisniku će biti prikazani detalji tog događaja.

#### Izmjena podataka o događaju

Izabrati opciju *Upravljanje događajima* iz glavnog menija. Nakon izbora događaja za izmjenu i pritiska na taster *RETURN* korisniku će biti prikazan novi prozor sa detaljima o događaju. Pritiskom tastera *F9*, korisniku se prikazuje novi ekran gdje korisnik bira polja koje želi da izmjeni. Po izboru polja za izmjenu, korisnik unosi novu vrijednost za to polje.

#### Brisanje događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija. Nakon izbora događaja za brisanje i pritiska na taster *DELETE* korisniku će biti prikazan novi prozor sa potvrdom brisanja. Potrebno je izabrati taster *F9* za potvrdu brisanja, ili taster *F10*, ako se sa brisanjem ne želi nastaviti.

### Upravljanje kategorijama

Upravljanje kategorijama je omogućeno registrovanim korisnicima.

#### Pregled kategorija

Izabrati opciju *Upravljanje kategorijama* iz glavnog menija.

#### Dodavanje nove kategorije događaja

Izabrati opciju *Upravljanje kategorijama* iz glavnog menija. Po otvaranju liste kategorija pritiskom na taster *F9* korisniku se prikazuje novi ekran gdje je potrebno unijeti podatke o novoj kategoriji. Nakon unosa naziva kategorije, kategorija će biti sačuvana i dodata u listu svih kategorija na koju će korisnik biti preusmjeren.

#### Brisanje kategorije događaja

Izabrati opciju *Upravljanje kategorijama* iz glavnog menija. Nakon izbora kategorije za brisanje i pritiska na taster *DELETE* korisniku će biti prikazan novi ekran sa potvrdom brisanja. Potrebno je izabrati taster *F9* za potvrdu brisanja, u suprtonom *F10*.


## Uputstvo za korištenje - SudoguUser

### Pregled današnjih događaja

Izabrati opciju *Pregled današnjih događaja* iz glavnog menija.

### Pregled događaja određene kategorije

Izabrati opciju *Pregled događaja određene kategorije* iz glavnog menija.

### Pregled svih budućih događaja

Izabrati opciju *Pregled svih budućih događaja* iz glavnog menija.

### Pregled događaja koji su prošli

Izabrati opciju *Pregled događaja koji su prošli* iz glavnog menija.

### Sortiranje događaja

Nakon otvaranja liste događaja i pritiska na taster *F10* pojavljuje se prozor gdje korisnik ima mogućnost izbora kolone po kojoj će se sortirati lista. Izborom kolone, korisniku se prikazuje sortirana lista događaja.

### Pregled detalja događaja

Po otvaranju liste događaja, pritiskom na taster *RETURN*, dobijaju se detalji odabranog događaja.

## Release History

* 1.0
    * Prva verzija aplikacije
