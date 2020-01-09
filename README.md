# OSI2019-Grupa-18 | Sudogu

> Sistem upravljanja događajima

Sudogu je sistem koji korisnicima omogućava pregled i upravljanje događajima u nekom gradu. Za događaje se mogu unositi podaci, mijenjati postojeći, brisati ili pregledati. Osim upravljanja podacima o događajima, sistem podržava i upravljanje podacima o vrstama događaja (kategorije), komentarisanje i igranje kviza o gradu.

Sistem se sastoji iz dva dijela: dijela za administraciju podataka i dijela za prikaz informacija. Dio za administraciju se prvenstveno koristi za unos, promjenu i brisanje podataka, dok se dio za prikaz informacija koristi samo za prikaz informacija o evidentiranim događanjima. Ova dva dijela su realizovani kao zasebne aplikacije.

Cilj sistema je da korisnicima obezbijedi lakše praćenje različitih događanja u gradu.

## Razvojno okruženje

Aplikacije su razvijene na Visual Studio 2019 (v142) korištenjem Windows SDK verzije 10.

## Uputstvo za instaliranje i pokretanje

Za instalaranje aplikacija dovoljno je preuzeti dva fajla: _setup.exe_ i _SudoguSetup.msi_. 

Instaliranje se obavlja pokretanjem instalacionog fajla _setup.exe_. U toku instalacije potrebno je odabrati direktorijum gdje će se aplikacije nalaziti. Nakon uspješne instalacije, aplikacije je moguće pokrenuti sa radne površine ili iz startnog menija.

Aplikacije se na nekim sistemima moraju pokrenuti sa administratorskim privilegijama.

## Uputstvo za korištenje - SudoguAdmin

### Sadržaj

- [Prijava na sistem](#prijava-na-sistem)
- [Registracija](#registracija)
- [Odjava sa sistema](#odjava-sa-sistema)
- [Upravljanje događajima](#upravljanje-doga%c4%91ajima)
    - [Pregled liste događaja](#pregled-liste-doga%c4%91aja)
    - [Dodvanje novog događaja](#dodvanje-novog-doga%c4%91aja)
    - [Sortiranje liste događaja](#sortiranje-liste-doga%c4%91aja)
    - [Pregled detalja postojećeg događaja](#pregled-detalja-postoje%c4%87eg-doga%c4%91aja)
    - [Izmjena podataka o događaju](#izmjena-podataka-o-doga%c4%91aju)
    - [Brisanje događaja](#brisanje-doga%c4%91aja)
- [Upravljanje kategorijama](#upravljanje-kategorijama)
    - [Pregled kategorija](#pregled-kategorija)
    - [Dodvanje nove kategorije događaja](#dodvanje-nove-kategorije-doga%c4%91aja)
    - [Brisanje kategorije događaja](#brisanje-kategorije-doga%c4%91aja)

### Prijava na sistem

Po otvoranju aplikacije korisnik ima mogućnost prijave na sistem. Potrebno je unijeti ispravno korisnično ime i lozinku da bi prijava bila moguća.

### Registracija

Registracija se ne obavlja kroz aplikaciju, već se nalog konfiguriše ručno. U instalacionom direktorijumu aplikacija, potrebno je izmjeniti fajl accounts.txt. Na kraj tog teksutalnog dokumenta, dodati nove podatke u formatu *korisničko_ime:lozinka* te sačuvati izmjene. Prilikom prvog sledećeg pokretanja apdministratorskog dijela sistema, konfiguracija se verifikuje. Ako su podaci uneseni u ispravnom obliku u datoteku, korisniku će biti omogućena prijava. U suprotnom, ispisuje se poruka o neispravnoj konfiguraciji naloga i program se zatvara. Korisnik neće biti u mogućnosti da koristi aplikaciju sve dok konfiguraciju ne ispravi.

### Odjava sa sistema

Izabrati opciju *Odjava* iz glavnog menija.

### Upravljanje događajima

Upravljanje događajima je omogućeno registrovanim korisnicima.

#### Pregled liste događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija.

#### Dodvanje novog događaja

Izabrati opciju *Upravljanje događajima* iz glavnog menija. Po otvaranju liste događaja pritiskom na taster *F9* korisniku se prikazuje novi ekran gdje je potrbno unijeti podatke o novom događaju. Pri izboru kategorije, korisnik mora da izabere neku od već postojećih kategorija. Nakon ispravnog unosa svih podataka o događaju, događaj će se sačuvati i dodati u listu svih događaja, na koju će korisnik biti preusmjeren.

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

#### Dodvanje nove kategorije događaja

Izabrati opciju *Upravljanje kategorijama* iz glavnog menija. Po otvaranju liste kategorija pritiskom na taster *F9* korisniku se prikazuje novi ekran gdje je potrbno unijeti podatke o novoj kategoriji. Nakon unosa naziva kategorije, kategorija će biti sačuvana i dodata u listu svih kategorija na koju će korisnik biti preusmjeren.

#### Brisanje kategorije događaja

Izabrati opciju *Upravljanje kategorijama* iz glavnog menija. Nakon izbora kategorije za brisanje i pritiska na taster *DELETE* korisniku će biti prikazan novi ekran sa potvrdom brisanja. Potrebno je izabrati taster *F9* za potvrdu brisanja, u suprtonom *F10*.


## Uputstvo za korištenje - SudoguUser

### Sadržaj

- [Pregled današnjih događaja](#pregled-dana%c5%a1njih-doga%c4%91aja)
- [Pregled događaja određene kategorije](#pregled-doga%c4%91aja-odre%c4%91ene-kategorije)
- [Pregled svih budućih događaja](#pregled-svih-budu%c4%87ih-doga%c4%91aja)
- [Pregled događaja koji su prošli](#pregled-doga%c4%91aja-koji-su-pro%c5%a1li)

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

## Meta

Branko Pekez – branko.pekez.peka@gmail.com

[https://github.com/Pynikleois](https://github.com/Pynikleois)