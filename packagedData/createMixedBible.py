import json
import xml.etree.ElementTree as ET
 
# -----------------------------------------------------------------------
# CONFIG - edit these paths
# -----------------------------------------------------------------------
KJV_JSON_PATH = "packagedData/data/KJV.json"          # existing KJV bible in the target JSON format
BRENTON_XML_PATH = "packagedData/data/lxxe.xml"    # Brenton's Septuagint in OSIS XML format
OUTPUT_PATH = "packagedData/data/bible.json"          # combined output file
 
 
# -----------------------------------------------------------------------
# Map OSIS book IDs -> full book names.
# Covers the 39 protocanonical OT books plus the common Apocrypha/
# Deuterocanon books that appear in Brenton's Septuagint.
# -----------------------------------------------------------------------
OSIS_BOOK_NAMES = {
    "Gen": "Genesis", "Exod": "Exodus", "Lev": "Leviticus", "Num": "Numbers",
    "Deut": "Deuteronomy", "Josh": "Joshua", "Judg": "Judges", "Ruth": "Ruth",
    "1Sam": "I Samuel", "2Sam": "II Samuel", "1Kgs": "I Kings", "2Kgs": "II Kings",
    "1Chr": "I Chronicles", "2Chr": "II Chronicles", "Ezra": "Ezra",
    "Neh": "Nehemiah", "Esth": "Esther", "Job": "Job", "Ps": "Psalms",
    "Prov": "Proverbs", "Eccl": "Ecclesiastes", "Song": "Song of Solomon",
    "Isa": "Isaiah", "Jer": "Jeremiah", "Lam": "Lamentations", "Ezek": "Ezekiel",
    "Dan": "Daniel", "Hos": "Hosea", "Joel": "Joel", "Amos": "Amos",
    "Obad": "Obadiah", "Jonah": "Jonah", "Mic": "Micah", "Nah": "Nahum",
    "Hab": "Habakkuk", "Zeph": "Zephaniah", "Hag": "Haggai", "Zech": "Zechariah",
    "Mal": "Malachi",
 
    # Common Apocrypha / Deuterocanonical books found in the LXX / Brenton
    # (left as-is since they have no KJV equivalent)
    "1Esd": "1 Esdras", "2Esd": "2 Esdras", "Tob": "Tobit", "Jdt": "Judith",
    "AddEsth": "Additions to Esther", "WisSol": "Wisdom of Solomon",
    "Wis": "Wisdom of Solomon", "Sir": "Sirach", "Bar": "Baruch",
    "EpJer": "Epistle of Jeremiah", "PrAzar": "Prayer of Azariah",
    "Sus": "Susanna", "Bel": "Bel and the Dragon", "PrMan": "Prayer of Manasseh",
    "1Macc": "1 Maccabees", "2Macc": "2 Maccabees", "3Macc": "3 Maccabees",
    "4Macc": "4 Maccabees", "Ps151": "Psalm 151", "AddDan": "Additions to Daniel",
}
 
# Names of the 27 New Testament books, in order, exactly as they appear in
# the KJV JSON's "name" field. Used to pull the NT books out of KJV JSON.
NT_BOOK_NAMES = [
    "Matthew", "Mark", "Luke", "John", "Acts", "Romans",
    "I Corinthians", "II Corinthians", "Galatians", "Ephesians",
    "Philippians", "Colossians", "I Thessalonians", "II Thessalonians",
    "I Timothy", "II Timothy", "Titus", "Philemon", "Hebrews",
    "James", "I Peter", "II Peter", "I John", "II John", "III John",
    "Jude", "Revelation of John",
]
 
 
def strip_ns(tag):
    """Remove an XML namespace prefix like '{http://...}chapter' -> 'chapter'."""
    return tag.split("}", 1)[-1] if "}" in tag else tag
 
 
def osis_id_part(osis_id, index):
    """Get a numeric piece of an osisID, e.g. osis_id_part('Gen.1.1', 2) -> 1."""
    # Some files use ranges like 'Gen.1.1-Gen.1.2'; just take the first part.
    first = osis_id.split("-")[0]
    parts = first.split(".")
    return int(parts[index])
 
 
def get_verse_text(verse_elem):
    """Collect all text inside a <verse> element, including text inside
    nested tags (like <w> for Strong's numbers), and clean up whitespace."""
    text = "".join(verse_elem.itertext())
    return " ".join(text.split())
 
 
def parse_brenton_xml(path):
    """Parse an OSIS XML file (container-style <div>/<chapter>/<verse>)
    and return a list of book dicts in the target JSON format."""
    tree = ET.parse(path)
    root = tree.getroot()
 
    books = []
 
    # Find every <div type="book" ...> anywhere in the document, regardless
    # of namespace (strip_ns handles the '{namespace}tag' form).
    for elem in root.iter():
        if strip_ns(elem.tag) != "div" or elem.get("type") != "book":
            continue
        book_div = elem
 
        osis_id = book_div.get("osisID")
        if not osis_id:
            continue
 
        book_name = OSIS_BOOK_NAMES.get(osis_id, osis_id)
 
        chapters = []
        for chapter_elem in book_div.iter():
            if strip_ns(chapter_elem.tag) != "chapter":
                continue
            chapter_osis_id = chapter_elem.get("osisID")
            if not chapter_osis_id:
                continue
            chapter_num = osis_id_part(chapter_osis_id, 1)
 
            verses = []
            for verse_elem in chapter_elem.iter():
                if strip_ns(verse_elem.tag) != "verse":
                    continue
                verse_osis_id = verse_elem.get("osisID")
                if not verse_osis_id:
                    continue
                verse_num = osis_id_part(verse_osis_id, 2)
                verse_text = get_verse_text(verse_elem)
 
                verses.append({
                    "verse": verse_num,
                    "text": verse_text,
                })
 
            chapters.append({
                "chapter": chapter_num,
                "verses": verses,
            })
 
        books.append({
            "name": book_name,
            "chapters": chapters,
        })
 
    return books
 
 
def main():
    # 1. Convert Brenton's XML -> list of OT book dicts
    print(f"Parsing Brenton XML from {BRENTON_XML_PATH} ...")
    ot_books = parse_brenton_xml(BRENTON_XML_PATH)
    print(f"  Found {len(ot_books)} books in Brenton's text.")
 
    # 2. Load the KJV JSON and pull out only the NT books
    print(f"Loading KJV JSON from {KJV_JSON_PATH} ...")
    with open(KJV_JSON_PATH, "r", encoding="utf-8") as f:
        kjv_data = json.load(f)
 
    kjv_books_by_name = {b["name"]: b for b in kjv_data["books"]}
 
    nt_books = []
    for name in NT_BOOK_NAMES:
        if name in kjv_books_by_name:
            nt_books.append(kjv_books_by_name[name])
        else:
            print(f"  Warning: '{name}' not found in KJV JSON, skipping.")
 
    print(f"  Found {len(nt_books)} NT books in KJV.")
 
    # 3. Combine OT (Brenton) + NT (KJV)
    combined = {
        "translation": "Old Testament: Brenton's Septuagint; New Testament: KJV (1769) with Strongs Numbers and Morphology and CatchWords",
        "books": ot_books + nt_books,
    }
 
    # 4. Write output
    with open(OUTPUT_PATH, "w", encoding="utf-8") as f:
        json.dump(combined, f, ensure_ascii=False, indent=4)
 
    print(f"Done. Wrote combined Bible to {OUTPUT_PATH} ({len(combined['books'])} books total).")
 
 
if __name__ == "__main__":
    main()
