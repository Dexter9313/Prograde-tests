from astroquery.jplhorizons import Horizons
import os
import math


# NAME, ID, TYPE, CENTER, SIDEREAL_ROTATION_PERIOD
planets=[
    ['Mercury', '199', 'majorbody', '500@10', 5067014.4],
    ['Venus', '299', 'majorbody', '500@10', 20997360.0],
    ['Earth', '399', 'majorbody', '500@10', 86164.0905],
    ['Moon', '301', 'majorbody', '500@10', 2360591.5104],
    ['Mars', '499', 'majorbody', '500@10', 88642.6848],
    ['Phobos', '401', 'majorbody', '500@10', 27553.843872],
    ['Deimos', '402', 'majorbody', '500@10', 109123.2],
    ['Jupiter', '599', 'majorbody', '500@10', 35730.0],
    ['Saturn', '699', 'majorbody', '500@10', 37980.0],
    ['Uranus', '799', 'majorbody', '500@10', 62063.712],
    ['Neptune', '899', 'majorbody', '500@10', 58000.32, 2469806.5],
    ['Pluto', '999', 'majorbody', '500@10', 551856.672, 2488068.5],
]

for planet in planets:
    maxJD=float('inf')
    if len(planet) >= 6:
        maxJD=planet[5]

    obj = Horizons(id=planet[1], id_type=planet[2], location=planet[3], epochs={'start':'2000-01-01', 'stop':'2000-01-02', 'step':'1d'})
    elem = obj.elements()
    print("-=-=-=-=-=-=-=-=-=- " + elem['targetname'][0] + " -=-=-=-=-=-=-=-=-=-")

    period=math.ceil(elem['P'][0])
    print("P: " + str(period) + " days")

    if period > maxJD-2451544.5:
        period = maxJD-2451544.5

    factor=1
    while period / factor > 18000:
        factor += 1

    #FIND AUTUMNAL EQUINOX DAY
    obj = Horizons(id=planet[1], id_type=planet[2], location=planet[3], epochs={'start':'JD'+str(2451544.5), 'stop':'JD'+str(2451544.5+period), 'step':str(factor) + 'd'})
    vectors = obj.vectors()

    maxJD='0.0'
    maxX=-1.0
    for day in range(len(vectors)):
        if vectors['x'][day] > maxX:
            maxX = vectors['x'][day]
            maxJD = vectors['datetime_jd'][day]

    #FIND AUTUMNAL EQUINOX MINUTE
    obj = Horizons(id=planet[1], id_type=planet[2], location=planet[3], epochs={'start':'JD'+str(maxJD-factor), 'stop':'JD'+str(maxJD+factor), 'step':'1m'})
    vectors = obj.vectors()
    autumnalDate=""
    maxX=-1.0
    for minute in range(len(vectors)):
        if vectors['x'][minute] > maxX:
            maxX = vectors['x'][minute]
            maxJD = vectors['datetime_jd'][minute]
            autumnalDate = vectors['datetime_str'][minute]

    print('Autumnal Equinox JD: ' + str(maxJD) + " - " + autumnalDate)

    universalTime=(maxJD-2451544.5)*24*3600
    print('UT=' + str(universalTime))

    #COMPUTE SIDEREAL TIME AT PRIME MERIDIAN ON AUTUMNAL EQUINOX

    obj = Horizons(id='10', id_type='majorbody', location={'lon':0.0, 'lat':0.0, 'elevation':0.0, 'body':planet[1]}, epochs=maxJD)
    #print(obj.ephemerides()['AZ', 'EL'])

    #NEXTLINE : FALSE - DO REAL TRANSFORMATION

    # We want longitude of the Sun
    # See https://en.wikipedia.org/wiki/Spherical_trigonometry
    # B = 90°- AZ ; c = 90° - EL ; C = 90°
    # take sin rules; b=asin(sin(c)*sin(B))
    # take eq3, C=90° so cos(c) = cos(a)*cos(b) ; cos(a) = cos(c) / cos(b)
    # a := longitude of sun on equator

    B=math.radians(90-obj.ephemerides()['AZ'][0])
    c=math.radians(90-obj.ephemerides()['EL'][0])
    b=math.asin(math.sin(c) * math.sin(B))
    a=math.acos(math.cos(c)/math.cos(b))
    #print(math.degrees(a))
    if B < -0.5*math.pi or B > 0.5*math.pi:
        a = math.pi*2.0 - a
    #print('foo')
    #print(math.degrees(b))
    #print(math.degrees(B))
    #print(math.degrees(c))
    #print(math.degrees(a))

    autSiderealTime=180.0-math.degrees(a)
    print("Autumnal sidereal time : " + str(autSiderealTime) + "°")
    sidRotPeriod=360.0/planet[4] #degrees per sec
    siderealTimeAtEpoch=autSiderealTime-(universalTime*sidRotPeriod)

    while siderealTimeAtEpoch >= 360.0:
        siderealTimeAtEpoch -= 360.0
    while siderealTimeAtEpoch < 0.0:
        siderealTimeAtEpoch += 360.0

    print("Sidereal Time at Epoch : " + str(siderealTimeAtEpoch) + "° = " + str(math.radians(siderealTimeAtEpoch)) + " rad")
    print("\n")

