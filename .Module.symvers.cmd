cmd_/home/asmar/RTOS/collective_assignment/Module.symvers := sed 's/\.ko$$/\.o/' /home/asmar/RTOS/collective_assignment/modules.order | scripts/mod/modpost -m -a  -o /home/asmar/RTOS/collective_assignment/Module.symvers -e -i Module.symvers   -T -