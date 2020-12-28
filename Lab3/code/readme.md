## Useful Commands in Mongodb
### To Control (Linux)
https://docs.mongodb.com/manual/tutorial/install-mongodb-on-ubuntu/

    1- Check your system spec.
        ps --no-headers -o comm 1
    2- if systemd
        sudo systemctl start mongod
        sudo systemctl status mongod
        sudo systemctl stop mongod
        sudo systemctl restart mongod
        mongo .. to enter the SHELL


### CRUD operations
https://docs.mongodb.com/manual/crud/


### Note:
    The restaurants.json needed a little modification before we start, adding commas and [].
    The "cuisine" "American " has a space at the end. VIP.