import xml.dom.minidom as minidom  

def parse_text_file(file_path):
    data = []
    with open(file_path, 'r') as file:
        next(file)  
        for line in file:
            values = line.strip().split('\t')
            time = values[0]
            latitude = values[1]
            longitude = values[2]
            altitude = values[7]  
            data.append((time, latitude, longitude, altitude))
    return data

def format_time(time_str):
    parts = time_str.split(":")
    formatted_time = "{:02d}:{:02d}:{:02d}".format(int(parts[0]), int(parts[1]), int(parts[2]))
    return "2024-03-20T{}Z".format(formatted_time)  

def create_xml(data):
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

    for entry in data:
        time, latitude, longitude, altitude = entry

        trkpt = doc.createElement("trkpt")
        trkpt.setAttribute("lat", latitude)
        trkpt.setAttribute("lon", longitude)
        trkseg.appendChild(trkpt)

        time_element = doc.createElement("time")
        time_element.appendChild(doc.createTextNode(format_time(time)))
        trkpt.appendChild(time_element)

        ele = doc.createElement("ele")
        ele.appendChild(doc.createTextNode(altitude))
        trkpt.appendChild(ele)

    return doc.toprettyxml(indent="  ")

def save_xml(xml_str, output_file):
    with open(output_file, "w") as file:
        file.write(xml_str)

def main(input_file, output_file):
    data = parse_text_file(input_file)
    xml_str = create_xml(data)
    save_xml(xml_str, output_file)

if __name__ == "__main__":
    input_file = "data.txt"  
    output_file = "output.gpx"  
    main(input_file, output_file)
