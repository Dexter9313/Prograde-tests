from astroquery.jplhorizons import Horizons
import os


# list of name; JPL Horizons ID; type; reference body
retrieve_list = [
#        ["Mercury", '199', 'majorbody', '500@10'],

#        ["Venus", '299', 'majorbody', '500@10'],

#        ["Earth", '399', 'majorbody', '500@10'],
#        ["Moon", '301', 'majorbody', '500@399'],

#        ["Mars", '499', 'majorbody', '500@10'],
#        ["Phobos", '401', 'majorbody', '500@499'],
#        ["Deimos", '402', 'majorbody', '500@499'],

#        ["Ceres", '1', 'smallbody', '500@10'],

#        ["Jupiter", '599', 'majorbody', '500@10'],
#        ['Io', '501', 'majorbody', '500@599'],
#        ['Europa', '502', 'majorbody', '500@599'],
#        ['Ganymede', '503', 'majorbody', '500@599'],
#        ['Callisto', '504', 'majorbody', '500@599'],

        ["Saturn", '699', 'majorbody', '500@10'],
        ["Mimas", '601', 'majorbody', '500@699'],
        ["Enceladus", '602', 'majorbody', '500@699'],
        ["Tethys", '603', 'majorbody', '500@699'],
        ["Dione", '604', 'majorbody', '500@699'],
        ["Rhea", '605', 'majorbody', '500@699'],
        ["Titan", '606', 'majorbody', '500@699'],
        ["Iapetus", '608', 'majorbody', '500@699'],

#        ["Uranus", '799', 'majorbody', '500@10'],

#        ["Neptune", '899', 'majorbody', '500@10'],

#        ["Pluto", '999', 'majorbody', '500@10'],
]


# list of start; stop; step
epochs_list = []
center=2024 # focus on year 2024
span=512

while span >= 16:
    epochs_list.append([str(center - 2*span), str(center - span)])
    epochs_list.append([str(center + span), str(center + 2*span)])
    span = int(span / 2)
epochs_list.append([str(center-16), str(center + 16)])


for epochs in epochs_list:
    # have around 7800 lines per file
    step=int(round((int(epochs[1])-int(epochs[0]))*365.25/7800.0))
    if step < 1:
        step = '1'
    else:
        step = str(step)
    epochs.append(step)

for planet in retrieve_list:
    if not os.path.exists(planet[0]):
        os.mkdir(planet[0])

    for epochs in epochs_list:
        start=epochs[0]
        stop=epochs[1]
        step=epochs[2]

        print(start)
        print(stop)
        print(step)

        obj = Horizons(id=planet[1], id_type=planet[2], location=planet[3], epochs={'start':start+'-01-01', 'stop':stop+'-01-01', 'step':step+'d'})
        try:
            elem = obj.elements()
        except ValueError:
            # no data available, just continue
            continue;

        print(elem['targetname'][0])

        elem['datetime_jd'] -= 2451544.5; # 1 jan 2000 00h00:00.00
        elem['incl'].convert_unit_to('rad')
        elem['Omega'].convert_unit_to('rad')
        elem['w'].convert_unit_to('rad')
        elem['a'].convert_unit_to('m')
        elem['M'].convert_unit_to('rad')


        table = elem['datetime_jd', 'incl', 'Omega', 'w', 'e', 'a', 'M']
        table.pprint(max_lines=10)

        table.write(planet[0] + '/' + planet[0] + '-' + start + '-' + stop + '-' + step + '.csv', format='ascii.csv', overwrite=True)

