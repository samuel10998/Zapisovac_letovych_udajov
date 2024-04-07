import xml.dom.minidom as minidom  

def parsuj_textovy_subor(file_path):
    data = []
    with open(file_path, 'r') as subor:
        next(subor)  # Preskočí hlavičku
        for riadok in subor:
            hodnoty = riadok.strip().split('\t')
            cas = hodnoty[0]  # Čas
            zem_sirka = hodnoty[1]  # Zemepisná šírka
            zem_dlzka = hodnoty[2]  # Zemepisná dĺžka
            vyska = hodnoty[7]  # Výška
            data.append((cas, zem_sirka, zem_dlzka, vyska))
    return data

def formatuj_cas(text_casu):
    parts = text_casu.split(":")
    formatovany_cas = "{:02d}:{:02d}:{:02d}".format(int(parts[0]), int(parts[1]), int(parts[2]))
    return "2024-03-20T{}Z".format(formatovany_cas)  

def vytvor_xml(data):
    doc = minidom.Document()
    gpx = doc.createElement("gpx")
    gpx.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
    gpx.setAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd")
    gpx.setAttribute("version", "1.1")
    gpx.setAttribute("creator", "Your Creator")
    gpx.setAttribute("xmlns", "http://www.topografix.com/GPX/1/1")
    doc.appendChild(gpx)

    trk = doc.createElement("trk")
    gpx.appendChild(trk)

    trkseg = doc.createElement("trkseg")
    trk.appendChild(trkseg)

    for zaznam in data:
        cas, zem_sirka, zem_dlzka, vyska = zaznam

        trkpt = doc.createElement("trkpt")
        trkpt.setAttribute("lat", zem_sirka)
        trkpt.setAttribute("lon", zem_dlzka)
        trkseg.appendChild(trkpt)

        cas_element = doc.createElement("time")
        cas_element.appendChild(doc.createTextNode(formatuj_cas(cas)))
        trkpt.appendChild(cas_element)

        ele = doc.createElement("ele")
        ele.appendChild(doc.createTextNode(vyska))
        trkpt.appendChild(ele)

    return doc.toprettyxml(indent="  ")

def uloz_xml(xml_retazec, vystupny_subor):
    with open(vystupny_subor, "w") as subor:
        subor.write(xml_retazec)

def main(vstupny_subor, vystupny_subor):
    data = parsuj_textovy_subor(vstupny_subor)
    xml_retazec = vytvor_xml(data)
    uloz_xml(xml_retazec, vystupny_subor)

if __name__ == "__main__":
    vstupny_subor = "data.txt"  
    vystupny_subor = "vystup.gpx"  
    main(vstupny_subor, vystupny_subor)
